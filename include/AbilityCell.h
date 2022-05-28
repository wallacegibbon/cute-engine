#pragma once

#include "Ability.h"
#include "GUI.h"
#include "Panel.h"
#include "Vendor.h"

class QGraphicsItem;
class QGraphicsPixmapItem;
class QPointF;

namespace cute {

class Panel;

/// Represents a GUI that visualizes and allows mouse interaction with an Ability.
/// When the Ability in the AbilityCell is clicked, AbilityCell will emit a signal.
/// @author Abdullah Aghazadah
class AbilityCell : public GUI {
    Q_OBJECT

public:
    AbilityCell(int width, int height, Ability *ability = nullptr);

    void set_ability(Ability *ability);
    Ability *ability() { return ability_; }

    void set_size(int width, int height);

    /// This works exactly the same as ItemCell::set_background_color().
    void set_background_color(const QColor &color) { background_->set_background_color(color); }
    void set_background_pixmap(const QPixmap &pixmap) { background_->set_background_pixmap(pixmap); }

    QGraphicsItem *get_graphics_item() override { return background_->get_graphics_item(); }

public slots:
    void on_clicked(Panel *panel, QPointF pos, int button) { emit clicked(this, button); }

signals:
    void clicked(AbilityCell *abilityCell, int button);

private:
    std::unique_ptr<QGraphicsPixmapItem> picture_;
    std::unique_ptr<Panel> background_;
    QPointer<Ability> ability_;

    void draw();
};

} // namespace cute
