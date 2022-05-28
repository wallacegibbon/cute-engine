#pragma once

#include "GUI.h"
#include "Inventory.h"
#include "ScrollWindow.h"
#include "Vendor.h"

class QPointF;

namespace cute {

class ItemCell;
class Item;
class ScrollWindow;

/// Represents a GUI element that visualizes and allows mouse interaction with
/// an Inventory. When an item in the inventory viewer is clicked, the
/// InventoryViewer emits a signal.
class InventoryViewer : public GUI {
    Q_OBJECT
public:
    InventoryViewer(Inventory *inventory = nullptr);

    QGraphicsItem *get_graphics_item() override;

    void set_inventory(Inventory *inventory);
    Inventory *inventory();

    void set_border(double amount);
    void set_padding_between_cells(double amount);
    void set_background_color(const QColor &color);
    void set_background_pixmap(const QPixmap &pixmap);
    void set_cell_background_color(const QColor &color);
    void set_cell_background_pixmap(const QPixmap &pixmap);
    void set_num_cells_horizontally(int numCellsH);
    void set_num_cells_vertically(int numCellsV);
    void set_cell_width(double width);
    void set_cell_height(double height);

    double height();
    double width();

public slots:
    void on_item_added_or_removed_from_inventory(Item *item);
    void on_item_cell_clicked(ItemCell *item_cell, int button);
    void on_inventory_destroyed();

signals:
    void item_clicked(Item *item, int button);

private:
    void draw();
    void draw_one_cell(Item *item, int x, int y);

private:
    double border_ = 15;
    double padding_between_cells_ = 5;
    int num_cells_horizontally_ = 3;
    int num_cells_vertically_ = 3;
    double cell_width_ = 64;
    double cell_height_ = 64;

    /// cell options
    bool is_cell_background_color_ = false;
    QColor cell_background_color_;
    QPixmap cell_background_pixmap_;

    std::unique_ptr<ScrollWindow> scroll_window_;
    QPointer<Inventory> inventory_;
    std::vector<ItemCell *> cells_;
};

} // namespace cute
