mod file_data;
mod layout;

pub use file_data::UnsizedDataFile;
pub use file_data::maincpp;
pub use layout::strings::MiniStringRef;

use core::str;

// Static Thread-wide Variables
thread_local! {
    pub static WORD_STRINGS: UnsizedDataFile<'static, str, MiniStringRef> =
        UnsizedDataFile::from_path("../../../db/links.bin", "../../../db/link_offsets.bin");
}
