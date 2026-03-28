use core::marker::Sized;
use core::mem::transmute;
use core::ops::Deref;
use core::{debug_assert_eq, marker, slice};
use i24::U24;
use marker::PhantomData;
use memmap2::Mmap;
use std::fs::File;
use std::path::Path;

#[cxx::bridge(namespace = "ffi")]
mod ffi {
    extern "Rust" {
        type LinkData<'a>;

        unsafe fn from_path<'a>(data_path: &'a str, offsets_path: &'a str) -> Box<LinkData<'a>>;

        unsafe fn index<'a>(link_data: &'a LinkData, index: usize) -> &'a [[u8; 3]];
    }

    unsafe extern "C++" {

        include!("wiki-solve/cpp/link_data.hpp");

        fn test();
    }
}

pub use ffi::test;

/// Specific LinkData type from UnsizedDataFile generics for cpp interoperability
type LinkData<'a> = UnsizedDataFile<'a, [u8; 3], usize>;

fn from_path<'a>(data_path: &'a str, offsets_path: &'a str) -> Box<LinkData<'a>> {
    Box::new(LinkData::from_path(data_path, offsets_path))
}

fn index<'a>(link_data: &'a LinkData, index: usize) -> &'a [[u8; 3]] {
    &link_data[index]
}

/// Represents data from files that store either a contigous list of:
/// A) Unsized entries (e.g., str)
/// B) Unsized slices of sized entries (e.g., [u32])
/// UnsizedDataFile uses two files: A contigous data file with unsized
/// data, and an offsets file with sized data used to index into the
/// contigous data file.
pub struct UnsizedDataFile<'a, D: ?Sized, O>
where
    O: AsIndex,
{
    pub data: Mmap,
    pub offsets: SizedDataFile<'a, O>,
    phantom_data: &'a PhantomData<D>,
    phantom_offset: PhantomData<O>,
}

/// Index into data slice using offset slice
pub trait AsIndex {
    fn as_index(&self) -> usize;
}

impl<'a, D: ?Sized, O: AsIndex> UnsizedDataFile<'a, D, O> {
    pub fn from_path<P: AsRef<Path>>(data_path: P, offsets_path: P) -> Self {
        let data_file = File::open(data_path).unwrap();

        let data_mmap = unsafe { Mmap::map(&data_file).unwrap() };

        Self {
            data: data_mmap,
            offsets: SizedDataFile::from_path(offsets_path),
            phantom_data: &PhantomData,
            phantom_offset: PhantomData,
        }
    }

    pub fn as_data_slice(&self) -> &'a [u8] {
        unsafe { transmute::<&[u8], &'a [u8]>(&*self.data) } // Safe because we know the mmap will last as long as the UnsizedDataFile
    }
}

/// Represents data from files that store a contigous list of
/// sized data of type D
pub struct SizedDataFile<'a, D> {
    data_ref: &'a [D],
    #[allow(dead_code)]
    data: Mmap,
}

impl<'a, D> Deref for SizedDataFile<'a, D> {
    type Target = [D];

    fn deref(&self) -> &Self::Target {
        self.data_ref
    }
}

impl<'a, D> SizedDataFile<'a, D> {
    pub fn from_path<P: AsRef<Path>>(data_path: P) -> Self {
        let data_file = File::open(data_path).unwrap();

        let data = unsafe { Mmap::map(&data_file).unwrap() };

        let data_ref = {
            let ptr = data.as_ptr() as *const D;
            let len = data.len() / size_of::<D>();

            unsafe { slice::from_raw_parts(ptr, len) }
        };

        Self { data_ref, data }
    }
}
