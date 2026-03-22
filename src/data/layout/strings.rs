use super::UnsizedDataFile;
use crate::data::WORD_STRINGS;
use crate::data::file_data::AsIndex;

use core::mem::transmute;
use core::ops::{Deref, Index};
use core::str::{self, from_utf8_unchecked};
use memmap2::Mmap;
use std::fs::File;
use std::io::{BufWriter, Write};

// MiniStringRef implementation
//
//
#[repr(C, packed)]
pub struct MiniStringRef {
    data_ptr: [u8; 4],
    len: u8,
}

impl AsIndex for MiniStringRef {
    fn as_index(&self) -> usize {
        u32::from_le_bytes(self.data_ptr) as usize
    }
}

impl AsRef<str> for MiniStringRef {
    fn as_ref(&self) -> &str {
        let str_index: usize = self.as_index();
        let len: usize = self.len as usize;

        let data_file_offset = WORD_STRINGS.with(|value| value.data.as_ptr());

        unsafe { str::from_raw_parts(data_file_offset.add(str_index), len) }
    }
}

// Specific Index function for str, since it is unsized
impl<'a, O: AsIndex> Index<usize> for UnsizedDataFile<'a, str, O> {
    type Output = str;

    fn index(&self, index: usize) -> &Self::Output {
        let data_slice = self.as_data_slice();
        let offsets = &self.offsets;

        if index >= offsets.len() {
            // Out of bounds
            panic!("FileData index out of bounds");
        }

        let byte_slice: &[u8];

        if index == offsets.len() - 1 {
            // Last offset
            let start = offsets[index].as_index();

            byte_slice = &data_slice[start..];
        } else {
            let start = offsets[index].as_index();
            let end = offsets[index + 1].as_index();

            byte_slice = &data_slice[start..end];
        }

        unsafe { from_utf8_unchecked(byte_slice) }
    }
}

// File Data implementation
//
//
impl<'a> UnsizedDataFile<'a, u8, MiniStringRef> {
    pub fn convert() {
        let file = File::open("./db/words.txt").unwrap();
        let mmap = unsafe { Mmap::map(&file).unwrap() };

        let mut offsets = mmap
            .iter()
            .enumerate()
            .filter(|(_, byte)| **byte == b'\n')
            .map(|(offset, _)| offset)
            .chain(std::iter::once(mmap.len()));

        let offset_file = File::create("./db/word_offsets_32.bin").unwrap();
        let string_file = File::create("./db/compressed_words_2.bin").unwrap();

        let mut offset_buffer = BufWriter::new(offset_file);
        let mut string_buffer = BufWriter::new(string_file);

        let mut old_offset: usize = 0;
        let _ = offset_buffer.write_all(&0u32.to_le_bytes());

        for offset in offsets.by_ref().take(1) {
            let _ = string_buffer.write_all(&mmap[old_offset..offset]);

            let bytes: [u8; 4] = (offset as u32).to_le_bytes();

            let _ = offset_buffer.write_all(&(offset - old_offset).to_le_bytes()[0..1]); // Write len
            let _ = offset_buffer.write_all(&bytes); // Write byte offset
            old_offset = offset;
        }

        for (i, offset) in offsets.enumerate() {
            let _ = string_buffer.write_all(&mmap[old_offset + 1..offset]);

            let bytes: [u8; 4] = ((offset - i - 1) as u32).to_le_bytes();

            let _ = offset_buffer.write_all(&(offset - old_offset - 1).to_le_bytes()[0..1]); // Write len
            let _ = offset_buffer.write_all(&bytes); // Write byte offset
            old_offset = offset;
        }
    }
}
