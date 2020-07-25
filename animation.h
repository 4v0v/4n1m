#pragma once

#include "mainwindow.h"

class Animation: public QWidget
{
    Q_OBJECT

signals:
    void begin_saving();
    void end_saving();

public:
    Animation();

    struct frame {
        bool is_empty = true;
        QImage image = QImage(1, 1, QImage::Format_ARGB32);
        QRect dimensions;
    };

    struct layer {
        int opacity = 255;
        QString name;
        QMap<int, frame> frames;
    };

    bool is_anim_empty();
    bool is_layer_empty(int l){ return layers.find(l)->frames.count() == 0; }
    bool is_frame_at(int l, int p);
    void add_layer_at(int l, layer la) { layers.insert(l, la); }
    void add_frame_at(int l, int p, frame f) { layers.find(l)->frames.insert(p, f); }
    void remove_layer_at(int l) { layers.remove(l); }
    void clear_layer_at(int l);
    void clear_animation();
    void remove_frame_at(int l, int p) { layers.find(l)->frames.remove(p); }
    int get_first_pos(int l) { return layers.find(l)->frames.firstKey(); }
    int get_last_pos(int l) { return layers.find(l)->frames.lastKey(); }
    int get_last_anim_pos();
    int get_prev_pos(int l, int p);
    int get_next_pos(int l, int p);
    void resize_frame(frame* f, Direction direction, int size);
    void init_frame(frame* f, QPoint pos);
    void clear_frame(frame* f);
    void foreach_frame_pos(int l, std::function<void(int)> action, int begin = 0, int end = -1);
    void foreach_frame_pos_revert(int l, std::function<void(int)> action, int begin = 0, int end = -1);
    QPainter frame_painter;
    frame get_frame_at(int l, int p) { return layers.find(l)->frames.value(p); }
    frame get_prev_frame_at(int l, int p) { return get_frame_at(l, get_prev_pos(l, p)); }
    frame get_next_frame_at(int l, int p) { return get_frame_at(l, get_next_pos(l, p)); }
    QImage create_onions_at(int l, int p, bool loop, bool prev, bool next);
    void add_onion_layer(QImage* img, int l, int p, double opacity, QColor color);
    void export_animation();
    void save_animation(QString path);
    void load_animation(QString path);

    int FPS = 24;
    QSize dimensions = QSize(1200, 1000);
    QMap<int, layer> layers;
    QPainter onion_painter;
    QPainter onions_painter;

};
