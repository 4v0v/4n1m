#include "animation.h"
#include "editor.h"
#include "qminiz.h"
#include "gif.h"
#include <QtXml/QDomDocument>
#include <QtXml/QDomImplementation>

Animation::Animation(): QWidget(nullptr)
{
    add_layer_at(0, layer{});
    add_layer_at(1, layer{});
    add_layer_at(2, layer{});
}

bool Animation::is_animation_empty()
{
    if (layers.isEmpty()) return true;

    foreach(layer l, layers)
        if (!l.frames.isEmpty()) return false;

    return true;
};

bool Animation::has_frame_at(int l, int p)
{
    if (!layers.contains(l)) return false;

    return layers.find(l)->frames.contains(p);
}

int Animation::get_last_anim_pos()
{
    int last_pos = 0;

    auto active_layers = layers.keys();

    foreach(int l, active_layers)
        if (!is_layer_empty(l) && get_last_pos(l) > last_pos) last_pos = get_last_pos(l);

    return last_pos;
};

int Animation::get_prev_pos(int l, int p)
{
    int prev_pos = -1;

    if (is_animation_empty() || is_layer_empty(l) || p == -1) return prev_pos;

    for (auto i = layers.find(l)->frames.begin(); i != layers.find(l)->frames.end(); ++i)
    {
        if (i.key() >= p) break;
        prev_pos = i.key();
    }

    return prev_pos;
}

int Animation::get_next_pos(int l, int p)
{
    if (is_animation_empty() || is_layer_empty(l) || p >= get_last_pos(l) || p == -1) return -1;

    return layers.find(l)->frames.upperBound(p).key();
}

int Animation::get_recursive_prev_pos(int l, int p, int nb)
{
    int prev_pos = p;

    for (int i = 0; i < nb; ++i) prev_pos = get_prev_pos(l, prev_pos);

    return prev_pos;
}

int Animation::get_recursive_next_pos(int l, int p, int nb)
{
    int next_pos = p;

    for (int i = 0; i < nb; i++) next_pos = get_next_pos(l, next_pos);

    return next_pos;
}

void Animation::clear_layer_at(int l)
{
    foreach_frame_pos_revert(l, [this, l](int i) { remove_frame_at(l, i); });
}

void Animation::clear_animation()
{
    foreach(int l, layers.keys()) clear_layer_at(l);
}

void Animation::init_frame(frame* f, QPoint pos)
{
    f->dimensions.setRect(
        pos.x() - 25 > 0                   ? pos.x() - 25 : 0,
        pos.y() - 25 > 0                   ? pos.y() - 25 : 0,
        pos.x() + 25 < dimensions.width()  ? 50           : 50 - (pos.x() + 25 - dimensions.width()),
        pos.y() + 25 < dimensions.height() ? 50           : 50 - (pos.y() + 25 - dimensions.height())
    );

    QImage resized_canvas(f->dimensions.size(), QImage::Format_ARGB32);
    resized_canvas.fill(Qt::transparent);

    f->image    = resized_canvas;
    f->is_empty = false;
}

void Animation::clear_frame(frame* f)
{
    f->dimensions.setRect(0, 0, 1, 1);
    f->image    = QImage(1, 1, QImage::Format_ARGB32);
    f->is_empty = true;
}

void Animation::resize_frame(frame* f, Direction direction, int size)
{
    QPoint point = QPoint(0, 0);

    switch (direction)
    {
        case RIGHT: {
            f->dimensions.setRight(size + 50 < dimensions.width() ? size + 50 : dimensions.width());
            break;
        } case BOTTOM: {
            f->dimensions.setBottom(size + 50 < dimensions.height() ? size + 50 : dimensions.height());
            break;
        } case LEFT: {
            int tempLeft = f->dimensions.left();
            f->dimensions.setLeft(size - 50 > 0 ? size - 50 : 0);
            point = QPoint(std::abs(f->dimensions.left() - tempLeft), 0);
            break;
        } case TOP: {
            int tempTop = f->dimensions.top();
            f->dimensions.setTop(size - 50 > 0 ? size - 50 : 0);
            point = QPoint(0, std::abs(f->dimensions.top() - tempTop));
            break;
        }
    }

    QImage resized_image = QImage(f->dimensions.size(), QImage::Format_ARGB32);
    resized_image.fill(Qt::transparent);

    static QPainter painter;

    painter.begin(&resized_image);
    painter.drawImage(point, f->image);
    painter.end();

    f->image = resized_image;
}

void Animation::foreach_frame_pos(int l, std::function<void(int)> action, int begin, int end)
{
    if (is_animation_empty() || is_layer_empty(l)) return;

    if (end == -1) end = get_last_pos(l);

    if (begin > end) return;

    for (int i = begin; i <= end; ++i)
        if (has_frame_at(l, i)) action(i);
}

void Animation::foreach_frame_pos_revert(int l, std::function<void(int)> action, int begin, int end)
{
    if (is_animation_empty() || is_layer_empty(l)) return;

    if (end == -1) end = get_last_pos(l);

    if (begin > end) return;

    for (int i = end; i >= begin; --i)
        if(has_frame_at(l, i)) action(i);
}

QImage Animation::create_onionskins_at(int l, int p, bool loop, int prev, int next)
{
    QImage onion_skins = QImage(dimensions, QImage::Format_ARGB32);

    if (is_layer_empty(l)) return onion_skins;

    for (int i = prev; i > 0; i--)
        fill_final_onionskin_image(&onion_skins, l, get_recursive_prev_pos(l, p, i), .5 - .1 * i, Qt::blue);

    for (int i = 1; i <= next; i++)
        fill_final_onionskin_image(&onion_skins, l, get_recursive_next_pos(l, p, i), .5 - .1 * i, Qt::red);

    if (loop)
    {
        fill_final_onionskin_image(&onion_skins, l, get_last_pos(l) , .3, Qt::darkYellow);
        fill_final_onionskin_image(&onion_skins, l, get_first_pos(l), .3, Qt::darkGreen );
    }

    return onion_skins;
}

void Animation::fill_final_onionskin_image(QImage* final, int l, int p, float opacity, QColor color)
{
    if (is_animation_empty() || is_layer_empty(l) || !has_frame_at(l, p)) return;

    QImage without_opacity = get_frame_at(l, p).image.copy();
    QImage with_opacity    = get_frame_at(l, p).image.copy();

    with_opacity.fill(Qt::transparent);

    static QPainter painter;

    // draw skin
    painter.begin(&without_opacity);
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawRect(without_opacity.rect());
    painter.end();

    // set skin opacity
    painter.begin(&with_opacity);
    painter.setOpacity(opacity);
    painter.drawImage(0, 0, without_opacity);
    painter.end();

    // draw skin on top of final onion skin image
    painter.begin(final);
    painter.drawImage(get_frame_at(l, p).dimensions.topLeft(), with_opacity);
    painter.end();
}

void Animation::export_animation(QString filename)
{
    if (is_animation_empty()) return;

    auto fileinfo  = QFileInfo(filename);
    auto qt_string = filename;

    if (fileinfo.suffix() != ".gif") qt_string += ".gif";

    auto str          = qt_string.toStdString();
    auto gif_filename = str.c_str();

    GifWriter g;
    GifBegin(&g, gif_filename, dimensions.width(), dimensions.height(), 1, 8, true);

    // TODO: pragma omp
    for (int i = 0; i <= get_last_anim_pos(); ++i)
    {
        // create image
        QImage img       = QImage(dimensions, QImage::Format_ARGB32);
        QPainter painter(&img);

        painter.fillRect(img.rect(), Mw::editor->bg_color);

        auto layer_keys = layers.keys();
        auto ri         = layer_keys.crbegin();

        while (ri != layer_keys.crend())
        {
            if (is_layer_empty(*ri)) { ri++; continue; };

            if (has_frame_at(*ri, i)) {
                frame f = get_frame_at(*ri, i);
                painter.drawImage(f.dimensions.topLeft(), f.image);

            } else if (has_frame_at(*ri, get_prev_pos(*ri, i))) {
                frame f = get_prev_frame_at(*ri, i);
                painter.drawImage(f.dimensions.topLeft(), f.image);
            }

            ri++;
        }

        // create gif frame from image
        uint8_t frame[dimensions.width() * dimensions.height() * 4];

        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                auto pixel_color = img.pixelColor(x, y);
                int r = pixel_color.red();
                int g = pixel_color.green();
                int b = pixel_color.blue();
                int a = pixel_color.alpha();

                auto pixel = &frame[(y*dimensions.width()+x)*4];
                pixel[0] = r;
                pixel[1] = g;
                pixel[2] = b;
                pixel[3] = a;
            }
        }

        GifWriteFrame(&g, frame, dimensions.width(), dimensions.height(), 1, 8, true);
    }

    GifEnd(&g);
}

void Animation::save_animation(QString filename)
{
    QString temp_folder = "temp_4n1m_save";
    auto    fileinfo    = QFileInfo(filename);
    auto    path        = fileinfo.path();

    if (is_animation_empty()) return;

    if (QDir(temp_folder).exists()) return;

    QDir().mkdir(temp_folder);

    // create images
    // TODO: pragma omp
    QList<QString> file_names;

    foreach(int l, layers.keys())
    {
        foreach_frame_pos(l, [this, l, path, temp_folder, &file_names](int i) {
            QString filename = QString::fromUtf8((std::to_string(l) + "_" + std::to_string(i) + ".png").c_str());
            frame f = get_frame_at(l, i);
            f.image.save( temp_folder + "\\" + filename);
            file_names.append(filename);
        });
    }

    // Create qXML object
    QDomDocument doc("AnimDocument");

    QDomElement anim_node = doc.createElement("animation");
    doc.appendChild(anim_node);
    anim_node.setAttribute("name","_");
    anim_node.setAttribute("date","xx-xx-xxxx");

    // TODO: pragma omp
    foreach(int l, layers.keys())
    {
        QDomElement layer_node = doc.createElement("layer");
        anim_node.appendChild(layer_node);
        layer_node.setAttribute("id", l);

        foreach_frame_pos(l, [this, &doc, &layer_node, l](int i) {
            QDomElement frame_node = doc.createElement("frame");
            layer_node.appendChild(frame_node);

            frame f = get_frame_at(l, i);
            frame_node.setAttribute("pos", i);
            frame_node.setAttribute("name", (std::to_string(l) + "_" + std::to_string(i)).c_str());
            frame_node.setAttribute("x", f.dimensions.x());
            frame_node.setAttribute("y", f.dimensions.y());
        });
    }

    // Write qXML inside  file
    QFile xml_file( temp_folder + "\\anim.xml" );
    xml_file.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream stream( &xml_file );
    stream << doc.toString();

    xml_file.close();
    file_names.append("anim.xml");

    // Remove saved zip if already exist
    QFile::remove(path + filename);

    QMiniZ::compressFolder(path + filename, temp_folder, QStringList(file_names));

    QDir dir( temp_folder );

    dir.removeRecursively();
}

void Animation::load_animation(QString path)
{
    QString temp_folder = "temp_4n1m_load";

    // dezip file inside temporary folder
    if (!QMiniZ::uncompressFolder(path, temp_folder)) return;

    QDomDocument doc;
    // Load xml file as raw data

    QFile f(temp_folder + "\\anim.xml");

    // Set data into the QDomDocument before processing
    doc.setContent(&f);

    QDomElement animation_node = doc.documentElement();


    clear_animation();

    // TODO: pragma omp
    for(int i = 0; i < animation_node.childNodes().count(); i++)
    {
        QDomNode layer_node = animation_node.childNodes().at(i);
        for(int j = 0; j < layer_node.childNodes().count(); j++)
        {
            QDomNode frame_node = layer_node.childNodes().at(j);
            QString layer_pos   = layer_node.attributes().namedItem("id").nodeValue();
            QString filename    = frame_node.attributes().namedItem("name").nodeValue();
            QString frame_pos   = frame_node.attributes().namedItem("pos").nodeValue();
            QString x           = frame_node.attributes().namedItem("x").nodeValue();
            QString y           = frame_node.attributes().namedItem("y").nodeValue();
            QImage img          = QImage(temp_folder + "\\" + filename + ".png"); // créer qimage a partir du png

            // créer frame
            frame f;
            f.is_empty   = false;
            f.image      = img;
            f.dimensions = QRect(x.toInt(), y.toInt(), img.width(), img.height());

            add_frame_at(layer_pos.toInt(), frame_pos.toInt(), f);
        }
    }

    f.close();

    // remove temporary folder
    QDir dir( temp_folder );
    dir.removeRecursively();

    Mw::update_all();
};
