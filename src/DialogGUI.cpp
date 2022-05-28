#include "DialogGUI.h"
#include "Label.h"
#include "ScrollWindow.h"

using namespace cute;

DialogGUI::DialogGUI()
        : top_scroll_window_(new ScrollWindow()), bottom_scroll_window_(new ScrollWindow()),
          response_label_(new Label()) {

    /// set parent/child relationship up
    bottom_scroll_window_->set_parent_GUI(top_scroll_window_.get());
    response_label_->set_parent_GUI(top_scroll_window_.get());

    /// set positions //TODO: temp, move to draw function, and parametrize to allow
    /// user customization such as width, offset, etc..
    bottom_scroll_window_->set_GUI_pos(QPointF(0, top_scroll_window_->height()));
    response_label_->set_GUI_pos(QPointF(60, 35));

    /// set default look
    top_scroll_window_->set_background_pixmap(QPixmap(":/cute-engine-builtin/resources/graphics/misc/paper.png"));
    bottom_scroll_window_->set_background_pixmap(QPixmap(":/cute-engine-builtin/resources/graphics/misc/paper.png"));
}

void DialogGUI::set_response(Response *response) {
    /// make sure response is not nullptr
    assert(response != nullptr);

    current_response_ = response;
    draw();
}

/// Adds a Response to the DialogGUI. Make sure to add some choices for this
/// response as well (via DialogGUI::add_choice(Response* response, Choice* forChoice).
/// The choices of a response are shown when that response is being displayed.
void DialogGUI::add_response(Response *response) {
    /// make sure response is not nullptr
    assert(response != nullptr);

    response_to_choices_[response] = std::unordered_set<Choice *>();
}

/// Adds a Choice for the specified Response.
/// See DialogGUI and DialogGUI::add_response(Response*) for more info.
void DialogGUI::add_choice(Response *forResponse, Choice *choice) {
    /// make sure the response exists
    assert(response_to_choices_.count(forResponse) != 0);

    /// make sure the choice is not null
    assert(choice != nullptr);

    response_to_choices_[forResponse].insert(choice);
    draw();
}

/// Sets the Response that is shown when the specified Choice is clicked.
void DialogGUI::set_response_for_choice(Response *response, Choice *forChoice) {
    /// make sure the response exists
    assert(response_to_choices_.count(response) != 0);

    choice_to_response_[forChoice] = response;
    draw();
}

QGraphicsItem *DialogGUI::get_graphics_item() { return top_scroll_window_->get_graphics_item(); }

/// Executed whenever the label of a choice has been clicked.
/// Will find the response for the choice and set it as the current response.
/// Will also emit a signal notifying any listeners that the choice has been clicked.
void DialogGUI::on_choice_label_clicked(Label *label) {
    /// makes no sense if label is nullptr
    assert(label != nullptr);

    Choice *labelsChoice = label_to_choice_[label];
    Response *response = choice_to_response_[labelsChoice];
    current_response_ = response;

    emit choice_clicked(this, labelsChoice);

    draw();
}

void DialogGUI::draw() {
    if (current_response_ == nullptr) {
        return;
    }

    /// remove previous choice's labels
    bottom_scroll_window_->remove_all();
    label_to_choice_.clear();

    /// draw top scroll window and content
    response_label_->set_text(current_response_->text());

    /// draw bottom scroll window and content
    /// - get all the choices for current response
    /// - for each choice,
    ///      -create a label, put it in scroll window
    ///      -add label to labelToChoice map
    ///      -connect to click event of label
    std::unordered_set<Choice *> choices = response_to_choices_[current_response_];
    int i = 0;
    for (Choice *choice : choices) {
        Label *choices_label = new Label();
        choices_label->set_text(choice->text());
        bottom_scroll_window_->add(choices_label, QPointF(40, i * 25 + 25));
        i++;
        label_to_choice_[choices_label] = choice;
        connect(choices_label, &Label::clicked, this, &DialogGUI::on_choice_label_clicked);
    }
}
