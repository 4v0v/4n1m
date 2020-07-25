#include "animation.h"
#include <QtXml/QDomDocument>
#include <QtXml/QDomImplementation>

Animation::Animation(): QWidget(nullptr)
{
    add_layer_at(0, layer{});
    add_layer_at(1, layer{});
    add_layer_at(2, layer{});
}

bool Animation::is_anim_empty()
{
    if (layers.count() == 0) return true;
    foreach(layer l, layers) {
        if (l.frames.count() > 0) return false;
    }
    return true;
};

int Animation::get_last_anim_pos()
{
    int i = 0;
    foreach(int l, layers.keys()) {
        if (!is_layer_empty(l) && get_last_pos(l) > i) i = get_last_pos(l);
    }
    return i;
};

bool Animation::is_frame_at(int l, int p)
{
    if (!layers.contains(l)) return false;
    return layers.find(l)->frames.contains(p);
}

int Animation::get_prev_pos(int l, int p)
{
    if (is_anim_empty() || is_layer_empty(l) || p == -1) return -1;
    int temp = -1;
    for (auto i = layers.find(l)->frames.begin(); i != layers.find(l)->frames.end(); ++i) {
        if (i.key() >= p) break;
        temp = i.key();
    }
    return temp;
}

void Animation::clear_layer_at(int l)
{
    foreach_frame_pos_revert(l, [this, l](int i){
        remove_frame_at(l, i);
    });
}

void Animation::clear_animation()
{
    foreach(int l, layers.keys())
        clear_layer_at(l);
}

int Animation::get_next_pos(int l, int p)
{
    if (is_anim_empty() || is_layer_empty(l) || p >= get_last_pos(l) || p == -1) return -1;
    return layers.find(l)->frames.upperBound(p).key();
}

void Animation::init_frame(frame* f, QPoint pos)
{
    f->is_empty = false;
    f->dimensions.setRect(
        pos.x() - 25 > 0 ? pos.x() - 25 : 0,
        pos.y() - 25 > 0 ? pos.y() - 25 : 0,
        pos.x() + 25 < dimensions.width() ? 50 : 50 - (pos.x() + 25 - dimensions.width()),
        pos.y() + 25 < dimensions.height() ? 50 : 50 - (pos.y() + 25 - dimensions.height())
    );
    QImage resized_canvas(f->dimensions.size(), QImage::Format_ARGB32);
    resized_canvas.fill(Qt::transparent);
    f->image = resized_canvas;
}

void Animation::clear_frame(frame* f)
{
    f->is_empty = true;
    f->image = QImage(1, 1, QImage::Format_ARGB32);
    f->dimensions.setRect(0,0,1,1);
}

void Animation::resize_frame(frame* f, Direction direction, int size)
{
    QImage resized_image;
    switch (direction) {
        case RIGHT: {
            f->dimensions.setRight(size + 50 < dimensions.width() ? size + 50 : dimensions.width());
            resized_image = QImage(QSize(dimensions.width(), f->image.height()), QImage::Format_ARGB32);
            resized_image.fill(Qt::transparent);
            frame_painter.begin(&resized_image);
            frame_painter.drawImage(QPoint(0, 0), f->image);
            break;
        } case BOTTOM: {
            f->dimensions.setBottom(size + 50 < dimensions.height() ? size + 50 : dimensions.height());
            resized_image = QImage(f->dimensions.size(), QImage::Format_ARGB32);
            resized_image.fill(Qt::transparent);
            frame_painter.begin(&resized_image);
            frame_painter.drawImage(QPoint(0, 0), f->image);
            break;
         } case LEFT: {
            int tempLeft = f->dimensions.left();
            f->dimensions.setLeft(size - 50 > 0 ? size - 50 : 0);
            resized_image = QImage(f->dimensions.size(), QImage::Format_ARGB32);
            resized_image.fill(Qt::transparent);
            frame_painter.begin(&resized_image);
            frame_painter.drawImage(QPoint(std::abs(f->dimensions.left() - tempLeft), 0), f->image);
            break;
        } case TOP: {
            int tempTop = f->dimensions.top();
            f->dimensions.setTop(size - 50 > 0 ? size - 50 : 0);
            resized_image = QImage(f->dimensions.size(), QImage::Format_ARGB32);
            resized_image.fill(Qt::transparent);
            frame_painter.begin(&resized_image);
            frame_painter.drawImage(QPoint(0, std::abs(f->dimensions.top() - tempTop)), f->image);
            break;
        }
    }

    frame_painter.end();
    f->image = resized_image;
}

void Animation::foreach_frame_pos(int l, std::function<void(int)> action, int begin, int end)
{
    if (is_anim_empty() || is_layer_empty(l)) return;
    if (end == -1) end = get_last_pos(l);
    if (begin > end) return;
    for (int i = begin; i <= end; ++i) if(is_frame_at(l, i)) action(i);
}

void Animation::foreach_frame_pos_revert(int l, std::function<void(int)> action, int begin, int end)
{
    if (is_anim_empty() || is_layer_empty(l)) return;
    if (end == -1) end = get_last_pos(l);
    if (begin > end) return;
    for (int i = end; i >= begin; --i) if(is_frame_at(l, i)) action(i);
}

QImage Animation::create_onions_at(int l, int pos, bool loop, bool prev, bool next)
{
    QImage onion_skins = QImage(dimensions, QImage::Format_ARGB32);

    if (prev)
    {
        add_onion_layer(&onion_skins, l, get_prev_pos(l, get_prev_pos(l, pos)), 0.2, Qt::blue);
        add_onion_layer(&onion_skins, l, get_prev_pos(l, pos), 0.3, Qt::blue);
    }
    if (next)
    {
        add_onion_layer(&onion_skins, l, get_next_pos(l, pos), 0.3, Qt::red);
        add_onion_layer(&onion_skins, l, get_next_pos(l, get_next_pos(l, pos)), 0.2, Qt::red);
    }
    if (loop)
    {
        if (!is_layer_empty(l))
        {
            add_onion_layer(&onion_skins, l, get_last_pos(l), 0.3, Qt::darkYellow);
            add_onion_layer(&onion_skins, l, get_first_pos(l), 0.3, Qt::darkGreen);
        }
    }

    return onion_skins;
}

void Animation::add_onion_layer(QImage* img, int l, int p, double opacity, QColor color)
{
    if (is_anim_empty() || is_layer_empty(l) || !is_frame_at(l, p)) return;
    QImage copy = get_frame_at(l, p).image.copy();

    onion_painter.begin(&copy);
    onion_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    Mw::set_painter_colors(&onion_painter, color);
    onion_painter.drawRect(copy.rect());
    onion_painter.end();

    onions_painter.begin(img);
    onions_painter.setOpacity(opacity);
    onions_painter.drawImage(get_frame_at(l, p).dimensions.topLeft(), copy);
    onions_painter.end();
}

void Animation::export_animation()
{
    if (is_anim_empty()) return;

    emit begin_saving();

    if (QDir("anim").exists() ) {
        QDir dir("anim");
        dir.removeRecursively();
    }

    QDir().mkdir("anim");

    for (int i = 0; i <= get_last_anim_pos(); ++i) {
        QString filename = QString::fromUtf8(("img_" + std::to_string(i) + ".png").c_str());
        QImage img = QImage(dimensions, QImage::Format_ARGB32);
        QPainter painter(&img);

        // TODO: find better way to reverse iterate qlist
        QList<int> revert_list = layers.keys();
        for(int k=0, s=layers.keys().size(), max=(s/2); k<max; k++) revert_list.swap(k,s-(1+k));
        foreach(int l, revert_list) {
            if (is_layer_empty(l)) continue;

            if (is_frame_at(l, i)) {
                frame f = get_frame_at(l, i);
                painter.drawImage(f.dimensions.topLeft(), f.image);
            } else if (is_frame_at(l, get_prev_pos(l, i))) {
                frame f = get_prev_frame_at(l, i);
                painter.drawImage(f.dimensions.topLeft(), f.image);
            }
        }

        img.save("anim\\" + filename);
    }

    emit end_saving();
}

void Animation::save_animation(QString path)
{
    if (is_anim_empty()) return;

    if (QDir( path ).exists() ) {
        QDir dir( path );
        dir.removeRecursively();
    }

    QDir().mkdir(path);
    QDir().mkdir( path + "\\data");

    foreach(int l, layers.keys()) {
        foreach_frame_pos(l, [this, l, path](int i){
            QString filename = QString::fromUtf8((std::to_string(l) + "_" + std::to_string(i) + ".png").c_str());
            frame f = get_frame_at(l, i);
            f.image.save( path +"\\data\\" + filename);
        });
    }

    QDomDocument doc("AnimDocument");

    QDomElement anim_node = doc.createElement("animation");
    doc.appendChild(anim_node);
    anim_node.setAttribute("name","_");
    anim_node.setAttribute("date","xx-xx-xxxx");

    foreach(int l, layers.keys()) {
        QDomElement layer_node = doc.createElement("layer");
        anim_node.appendChild(layer_node);
        layer_node.setAttribute("id", l);
        foreach_frame_pos(l, [this, &doc, &layer_node, l](int i){
            QDomElement frame_node = doc.createElement("frame");
            layer_node.appendChild(frame_node);
            frame f = get_frame_at(l, i);
            frame_node.setAttribute("pos", i);
            frame_node.setAttribute("name", (std::to_string(l) + "_" + std::to_string(i)).c_str());
            frame_node.setAttribute("x", f.dimensions.x());
            frame_node.setAttribute("y", f.dimensions.y());
        });

    }

    QFile file( path + "\\anim.xml" );
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream stream( &file );
    stream << doc.toString();
    file.close();
}


void Animation::load_animation(QString path)
{
    // TODO: supprimer aussi layers
    clear_animation();

    QDomDocument doc;
    // Load xml file as raw data
    QFile f(path + "\\anim.xml");

    // Set data into the QDomDocument before processing
    doc.setContent(&f);

    QDomElement animation_node = doc.documentElement();

    for(int i = 0; i < animation_node.childNodes().count(); i++)
    {
        QDomNode layer_node = animation_node.childNodes().at(i);
        for(int j = 0; j < layer_node.childNodes().count(); j++)
        {
            QDomNode frame_node = layer_node.childNodes().at(j);

            QString layer_pos = layer_node.attributes().namedItem("id").nodeValue();
            QString filename =  frame_node.attributes().namedItem("name").nodeValue();
            QString frame_pos =  frame_node.attributes().namedItem("pos").nodeValue();
            QString x = frame_node.attributes().namedItem("x").nodeValue();
            QString y = frame_node.attributes().namedItem("y").nodeValue();

            // créer qimage a partir du png
            QImage img = QImage(path + "\\data\\" + filename + ".png");

            // créer frame
            frame f;
            f.is_empty = false;
            f.image = img;
            f.dimensions = QRect(x.toInt(), y.toInt(), img.width(), img.height());

            add_frame_at(layer_pos.toInt(), frame_pos.toInt(), f);
        }
    }

    f.close();
};
