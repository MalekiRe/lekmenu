pub mod app_path_handler;
pub mod entry;
pub mod keyboard_handler;

use app_path_handler::get_all_files;
use entry::Entry;
use glam::{vec3, Quat};
use keyboard_handler::KeyboardHandler;
use manifest_dir_macros::directory_relative_path;
use mint::Vector3;
use stardust_xr_molecules::{
	fusion::{
		client::Client,
		data::PulseReceiver,
		drawable::{Text, TextStyle},
		fields::SphereField,
		node::NodeType,
	},
	keyboard::KEYBOARD_MASK,
};
use std::f32::consts::PI;

fn get_matching_files(str_to_cmp: &str, files: &Vec<String>) -> Vec<String> {
	let mut matching = Vec::new();
	for file in files {
		if str_to_cmp.len() > file.len() {
			continue;
		}
		if &file.as_bytes()[..str_to_cmp.len().min(file.len())] == str_to_cmp.as_bytes() {
			matching.push(file.clone());
		}
	}
	matching
}
fn adjust_list(
	main_text: &mut Text,
	selected_option: &mut usize,
	entries: &mut Vec<Entry>,
	app: &mut String,
) {
	entries.clear();
	*selected_option = std::usize::MAX;
	if app.len() <= 1 {
		return;
	}
	let matching_paths = get_matching_files(&app, &get_all_files().unwrap());
	let new_entries = matching_paths
		.into_iter()
		.enumerate()
		.map(|(i, matching_path)| {
			let mut entry = Entry {
				text: main_text.alias(),
				text_string: matching_path,
				selected: false,
				origin: Vector3 {
					x: 0.0,
					y: -0.1 * (i + 1) as f32,
					z: 0.0,
				},
				offset_origin: Vector3 {
					x: 0.1,
					y: -0.1 * (i + 1) as f32,
					z: 0.0,
				},
			};
			if i == 0 {
				entry.set_selected();
				*selected_option = 0;
			}
			entry
		})
		.collect::<Vec<Entry>>();
	*entries = new_entries;
}

#[tokio::main(flavor = "current_thread")]
async fn main() {
	let (client, event_loop) = Client::connect_with_async_loop().await.unwrap();
	client.set_base_prefixes(&[directory_relative_path!("res")]);

	let mut main_string = "hi".to_string();
	let mut entries: Vec<Entry> = Vec::new();

	let style: TextStyle<String> = TextStyle {
		character_height: 0.1,
		..Default::default()
	};
	let mut main_text = Text::builder()
		.spatial_parent(client.get_root())
		.text_string("hi")
		.style(style)
		.position(vec3(0.1, 0.0, -0.1))
		.rotation(Quat::from_rotation_y(PI))
		.build()
		.unwrap();

	let field = SphereField::builder()
		.spatial_parent(client.get_root())
		.radius(0.1)
		.build()
		.unwrap();
	let _pulse_reciver =
		PulseReceiver::create(client.get_root(), None, None, &field, KEYBOARD_MASK.clone())
			.unwrap()
			.wrap(KeyboardHandler::new());
	tokio::select! {
		_ = tokio::signal::ctrl_c() => (),
		e = event_loop => e.unwrap().unwrap(),
	}
}
