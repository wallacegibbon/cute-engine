#pragma once

#include "GUI.h"
#include "ScrollWindow.h"
#include "Vendor.h"

class QGraphicsItem;

namespace cute {

class Response;
class Label;
class ScrollWindow;
class Label;

/// Represents a choice that can be presented in a DialogGUI when a certain response is presented.
class Choice {
public:
    Choice(const std::string &text) : text_(text) {}
    std::string text() const { return text_; }

private:
    std::string text_;
};

/// Represents a response that can be presented in a DialogGUI when a certain choice is selected
/// (or when no choice is selected - the default displayed Response).
class Response {
public:
    Response(const std::string &text) : text_(text) {}
    std::string text() { return text_; }

private:
    std::string text_;
};

/// Represents a GUI that can be used to present an interactive dialog.
///
/// Initially a "response" is displayed with all the "choices" for that response.
/// When a certain choice is clicked, the responses of that choice will be displayed,
/// along with all of the choices for that new response.
///
/// So each response has a bunch of choices that are displayed when that response is
/// displayed and each choice has a response that it will display when its clicked.
///
/// You can add a response via add_response(Response* response).
/// You can add a choices for a response via add_choice(Response* forResponse, Choice* choice).
/// Please note that you must add a Response via add_response() first before adding a choice for the response.
/// You can set the associated response that will be shown when a certain
/// choice is clicked by calling set_response_for_choice(Response* response, Choice* forChoice).
/// Please note that you must add a Choice via add_choice() before setting the response for the Choice.
/// You can set the currently displayed response via set_response(Response* response).
/// You should only call set_response(Response*) to set the initially displayed
/// Response. After that, the responses will change depending on the choices clicked.
/// DialogGUI will emit a signal whenever a certain choice is clicked.
///
/// The way to use DialogGUI would be:
/// - create a few Responses
/// - add the responses via add_response()
/// - create Choices for the Responses
/// - add the choices for their respective response via add_choice()
/// - set the initial Response to be displayed via set_response()
///
/// That's it! The initial response and all of its choices will be displayed.
/// When the user clicks on a choice, the response for that choice (along with the Response's
/// choices) will be displayed. By clicking the various Choices, the user can "talk" wit the dialog.
///
/// Here is a working example :)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// DialogGUI* dg = new DialogGUI();
/// Response* initial_text = new Response("Hello stranger, what brings thee here?");
/// Choice* initial_text_choice1 = new Choice("Screw you!");
/// Choice* initial_text_choice2 = new Choice("I'm looking for adventure!");
///
/// dg->add_response(initial_text);
/// dg->add_choice(initial_text,initial_text_choice1);
/// dg->add_choice(initial_text,initial_text_choice2);
/// dg->set_response(initial_text);
///
/// Response* screwYouResponse = new Response("You're manners are not very good...ATTACK!");
/// Response* adventureResponse = new Response("I salute thee! You should talk to our village elder for some quests.");
/// dg->add_response(screwYouResponse);
/// dg->add_response(adventureResponse);
/// dg->set_response_for_choice(screwYouResponse,initial_text_choice1);
/// dg->set_response_for_choice(adventureResponse,initial_text_choice2);
///
/// game->add_GUI(dg);
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
/// @author Abdullah Aghazadah
/// @date 11/2/16
class DialogGUI : public GUI {
    Q_OBJECT
public:
    DialogGUI();

    void set_response(Response *response);
    void add_response(Response *response);
    void add_choice(Response *forResponse, Choice *choice);
    void set_response_for_choice(Response *response, Choice *forChoice);

    QGraphicsItem *get_graphics_item() override;

public slots:
    void on_choice_label_clicked(Label *label);

signals:
    void choice_clicked(DialogGUI *dialog, Choice *choice);

private:
    void draw();

private:
    /// what are the choices for each resposne?
    std::unordered_map<Response *, std::unordered_set<Choice *>> response_to_choices_;

    /// what is the response for each choice?
    std::unordered_map<Choice *, Response *> choice_to_response_;

    /// remember the Label for each choice
    std::unordered_map<Label *, Choice *> label_to_choice_;

    /// what is the currently displayed response?
    Response *current_response_ = nullptr;

    /// GUIs
    std::unique_ptr<ScrollWindow> top_scroll_window_;
    ScrollWindow *bottom_scroll_window_;
    Label *response_label_;
};

} // namespace cute
