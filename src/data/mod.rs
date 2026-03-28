mod file_data;
mod layout;

pub use file_data::UnsizedDataFile;
pub use file_data::test;
pub use layout::strings::MiniStringRef;

use core::str;

// Static Thread-wide Variables
thread_local! {
    pub static WORD_STRINGS: UnsizedDataFile<'static, str, MiniStringRef> =
        UnsizedDataFile::from_path("./db/articles.bin", "./db/article_offsets.bin");
}
