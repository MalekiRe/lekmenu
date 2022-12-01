use anyhow::Result;
use std::env;
use walkdir::WalkDir;

pub fn get_all_files() -> Result<Vec<String>> {
	let path = env::var("PATH")?;
	let files = path
		.split(':')
		.flat_map(|token| WalkDir::new(token).follow_links(false))
		.filter_map(Result::ok)
		.map(|entry| entry.file_name().to_string_lossy().to_string())
		.collect();
	Ok(files)
}
