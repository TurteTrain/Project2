#![feature(str_from_raw_parts)]

// The program needs to do lots of 32-bit pointer arithmetic,
// so the minimum pointer width is 32.
const _: () = assert!(
    std::mem::size_of::<usize>() >= 4,
    "This program requires at least a 32-bit pointer width."
);

mod data;
use data::WORD_STRINGS;

use core::time::Duration;
use frizbee::Match;
use frizbee::{Config, match_list_parallel};
use std::time::Instant;

use enum_dispatch::enum_dispatch;

use iced::alignment::{Horizontal, Vertical};
use iced::widget::{container, text, text_input};
use iced::{Element, Length, Theme};

enum Message {}

struct State {
    view: Views,
    theme: Theme,
}

impl Default for State {
    fn default() -> Self {
        Self {
            view: SearchPanelView {}.into(),
            theme: Theme::Dark,
        }
    }
}

#[enum_dispatch]
enum Views {
    SearchView,
    SearchPanelView,
    SearchDiagramView,
}

#[enum_dispatch(Views)]
trait View<Message> {
    fn view(&self) -> Element<'_, Message>;
}

struct SearchView {
    panel_view: SearchPanelView,
    diagram_view: SearchDiagramView,
}

impl View<Message> for SearchView {
    fn view(&self) -> Element<'_, Message> {
        self.panel_view.view()
    }
}

struct SearchPanelView {}

fn search_bar(value: &str) -> Element<'_, Message> {
    //container(text_input("Enter Wikipedia Article Title or URL", value)).into()
    todo!()
}

impl View<Message> for SearchPanelView {
    fn view(&self) -> Element<'_, Message> {
        container(
            text("please donate")
                .align_x(Horizontal::Center)
                .align_y(Vertical::Center),
        )
        .width(Length::Fill)
        .height(Length::Fill)
        .align_x(Horizontal::Center)
        .align_y(Vertical::Center)
        .into()
    }
}

struct SearchDiagramView {}

impl View<Message> for SearchDiagramView {
    fn view(&self) -> Element<'_, Message> {
        todo!()
    }
}

// Application functions
fn boot() -> State {
    State::default()
}

fn update(state: &mut State, message: Message) {}

fn view(state: &State) -> Element<'_, Message> {
    state.view.view()
}

fn theme(state: &State) -> Option<Theme> {
    Some(state.theme.clone())
}

fn main() {
    data::maincpp();
    let app = iced::application(boot, update, view);
    app.theme(theme).run().expect("Application failed to run");

    // data::FileData::<u8, data::MiniStringRef>::convert();

    /*
    for i in 0..50000 {
        WORD_STRINGS.with(|test| println!("{}", test.offsets[i].as_ref()));
    }

    let needle = "Cherno";

    let mut config = Config::default();
    config.max_typos = Some(0);

    let mut elapsed: Option<Duration> = None;
    let mut matches: Option<Vec<Match>> = None;

    WORD_STRINGS.with(|test| {
        let timer = Instant::now();
        let haystacks = &test.offsets;
        matches = Some(match_list_parallel(needle, haystacks, &config, 8));
        elapsed = Some(timer.elapsed());
    });

    let elapsed = elapsed.unwrap();

    let matches = matches.unwrap();

    let match_str_iter = matches
        .iter()
        .enumerate()
        .map(|(i, mat)| {
            (
                i + 1,
                WORD_STRINGS.with(|test| test.offsets[mat.index as usize].as_ref().to_owned()),
            )
        })
        .rev();

    for (i, match_str) in match_str_iter {
        println!("{i}. {}\n", match_str);
    }
    println!("nanos elapsed: {}", elapsed.as_nanos());
    */
}
