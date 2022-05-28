#pragma once

#include "GUI.h"
#include "Item.h"
#include "Panel.h"
#include "Vendor.h"

class QGraphicsItem;
class QGraphicsPixmapItem;
class QColor;
class QPixmap;
class QGraphicsSimpleTextItem;

namespace cute {

class Panel;

/// Represents a GUI that visualizes and allows mouse interaction with an Item.
/// When the Item in the ItemCell is clicked, ItemCell will emit a signal.
class ItemCell : public GUI {
    Q_OBJECT

public:
    ItemCell(int width, int height, Item *item = nullptr);

    void set_item(Item *item);
    Item *item() { return item_; }

    void set_size(int width, int height);
    void set_background_color(const QColor &color);
    void set_background_pixmap(const QPixmap &pixmap);

    QGraphicsItem *get_graphics_item() override;

public slots:
    void on_background_clicked(Panel *panel, QPointF pos, int button);
    void on_item_charges_changed(Item *sender);

signals:
    void clicked(ItemCell *item_cell, int button);

private:
    std::unique_ptr<Panel> background_;
    std::unique_ptr<QGraphicsPixmapItem> picture_;
    QGraphicsSimpleTextItem *num_charges_text_;
    QPointer<Item> item_;

    void draw();
};

} // namespace cute
