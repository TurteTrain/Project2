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

fn main() {
    // data::FileData::<u8, data::MiniStringRef>::convert();

    WORD_STRINGS.with(|test| println!("{}", &test[234]));

    let needle = "rhinoce";

    let config = Config::default();
    //config.max_typos = Some(1);

    let mut elapsed: Option<Duration> = None;
    let mut matches: Option<Vec<Match>> = None;

    WORD_STRINGS.with(|test| {
        let timer = Instant::now();
        let haystacks = &test.offsets;
        matches = Some(match_list_parallel(needle, haystacks, &config, 8));
        elapsed = Some(timer.elapsed());
    });

    let elapsed = elapsed.unwrap();

    /*
    let closest_match =
        WORD_STRINGS.with(|test| &unsafe { test.as_slice() }[matches[0].index as usize]);*/

    let matches = matches.unwrap();

    let match_str_iter = matches
        .iter()
        .enumerate()
        .map(|(i, mat)| {
            (
                i + 1,
                WORD_STRINGS.with(|test| test[mat.index as usize].to_owned()),
            )
        })
        .rev();

    for (i, match_str) in match_str_iter {
        println!("{i}. {}\n", match_str);
    }
    println!("nanos elapsed: {}", elapsed.as_nanos());
}
