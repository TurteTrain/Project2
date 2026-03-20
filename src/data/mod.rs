mod file_data;
mod layout;

pub use file_data::UnsizedDataFile;
pub use layout::strings::MiniStringRef;

use core::str;

// Static Thread-wide Variables
thread_local! {
    pub static WORD_STRINGS: UnsizedDataFile<'static, str, MiniStringRef> =
        UnsizedDataFile::from_path("./db/compressed_words_2.bin", "./db/word_offsets_32.bin");
}
