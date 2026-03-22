use core::{
    convert::Into,
    ops::{Deref, Index},
    slice,
    str::from_utf8_unchecked,
};

use crate::data::file_data::AsIndex;

use super::UnsizedDataFile;

impl<T: Into<usize> + Copy> AsIndex for T {
    fn as_index(&self) -> usize {
        <Self as Into<usize>>::into(*self)
    }
}

// Blanket Index implementation for all unsized lists of sized data types (D)
impl<'a, D, O> Index<usize> for UnsizedDataFile<'a, D, O>
where
    O: AsIndex,
    D: Sized,
{
    type Output = [D];

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

        let ptr = byte_slice.as_ptr() as *const D;
        let len = byte_slice.len() * size_of::<D>();

        unsafe { slice::from_raw_parts(ptr, len) }
    }
}
