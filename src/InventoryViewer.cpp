#include "InventoryViewer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemCell.h"
#include "ScrollWindow.h"

using namespace cute;

InventoryViewer::InventoryViewer(Inventory *inventory)
        : cell_background_pixmap_(QPixmap(":/cute-engine-builtin/resources/graphics/misc/invcellbg.png")),
          inventory_(inventory), scroll_window_(new ScrollWindow()) {
    set_inventory(inventory);
    scroll_window_->set_background_pixmap(QPixmap(":/cute-engine-builtin/resources/graphics/misc/invbg.png"));
}

QGraphicsItem *InventoryViewer::get_graphics_item() { return scroll_window_->get_graphics_item(); }

void InventoryViewer::set_inventory(Inventory *inventory) {
    /// if currently viewing an inventory, stop viewing it
    if (inventory_ != nullptr) {
        disconnect(inventory_, &Inventory::item_added, this, &InventoryViewer::on_item_added_or_removed_from_inventory);
        disconnect(inventory_, &Inventory::item_removed, this,
                   &InventoryViewer::on_item_added_or_removed_from_inventory);
        disconnect(inventory_, &Inventory::destroyed, this, &InventoryViewer::on_inventory_destroyed);
    }

    /// listen for changes to this inventory
    if (inventory != nullptr) {
        connect(inventory, &Inventory::item_added, this, &InventoryViewer::on_item_added_or_removed_from_inventory);
        connect(inventory, &Inventory::item_removed, this, &InventoryViewer::on_item_added_or_removed_from_inventory);
        connect(inventory_, &Inventory::destroyed, this, &InventoryViewer::on_inventory_destroyed);
        inventory_ = inventory;
    }
    draw();
}

Inventory *InventoryViewer::inventory() { return inventory_; }

void InventoryViewer::set_border(double amount) {
    border_ = amount;
    draw();
}

void InventoryViewer::set_padding_between_cells(double amount) {
    padding_between_cells_ = amount;
    draw();
}

void InventoryViewer::set_background_color(const QColor &color) { scroll_window_->set_background_color(color); }

void InventoryViewer::set_background_pixmap(const QPixmap &pixmap) { scroll_window_->set_background_pixmap(pixmap); }

void InventoryViewer::set_cell_background_color(const QColor &color) {
    is_cell_background_color_ = true;
    cell_background_color_ = color;
    draw();
}

void InventoryViewer::set_cell_background_pixmap(const QPixmap &pixmap) {
    is_cell_background_color_ = false;
    cell_background_pixmap_ = pixmap;
    draw();
}

void InventoryViewer::set_num_cells_horizontally(int numCellsH) {
    num_cells_horizontally_ = numCellsH;
    draw();
}

void InventoryViewer::set_num_cells_vertically(int numCellsV) {
    num_cells_vertically_ = numCellsV;
    draw();
}

void InventoryViewer::set_cell_width(double width) {
    cell_width_ = width;
    draw();
}

void InventoryViewer::set_cell_height(double height) {
    cell_height_ = height;
    draw();
}

double InventoryViewer::height() {
    double bg_height =
            num_cells_vertically_ * (cell_height_ + padding_between_cells_) - padding_between_cells_ + 2 * border_;
    return bg_height;
}

double InventoryViewer::width() {
    double bg_width =
            num_cells_horizontally_ * (cell_width_ + padding_between_cells_) - padding_between_cells_ + 2 * border_;
    return bg_width;
}

void InventoryViewer::on_item_added_or_removed_from_inventory(Item *item) { set_inventory(inventory_); }

void InventoryViewer::on_item_cell_clicked(ItemCell *item_cell, int button) {
    Item *item = item_cell->item();
    if (item != nullptr) {
        emit item_clicked(item, button);
    }
}

void InventoryViewer::on_inventory_destroyed() { draw(); }

void InventoryViewer::draw() {
    /// draw background
    double bg_width = width();
    double bg_height = height();

    scroll_window_->set_width(bg_width);
    scroll_window_->set_height(bg_height);

    /// clear/stop listening to all previously drawn inventory cells
    for (ItemCell *cell : cells_) {
        disconnect(cell, &ItemCell::clicked, this, &InventoryViewer::on_item_cell_clicked);
        scroll_window_->remove(cell);
    }
    cells_.clear();

    if (inventory_ == nullptr) {
        return;
    }

    /// draw all items in new inventory cells
    int x = 0;
    int y = 0;
    for (Item *item : inventory_->get_items()) {
        draw_one_cell(item, x, y);
        x++;
        if (x == num_cells_horizontally_) {
            x = 0;
            y++;
        }
    }
}

void InventoryViewer::draw_one_cell(Item *item, int x, int y) {
    ItemCell *cell = new ItemCell(cell_width_, cell_height_, item);
    if (is_cell_background_color_) {
        cell->set_background_color(cell_background_color_);
    } else {
        cell->set_background_pixmap(cell_background_pixmap_);
    }

    double cell_x = x * (cell_width_ + padding_between_cells_) + border_;
    double cell_y = y * (cell_height_ + padding_between_cells_) + border_;
    cell->set_GUI_pos(QPointF(cell_x, cell_y));

    cells_.push_back(cell);
    scroll_window_->add(cell, QPointF(cell_x, cell_y));

    connect(cell, &ItemCell::clicked, this, &InventoryViewer::on_item_cell_clicked);
}