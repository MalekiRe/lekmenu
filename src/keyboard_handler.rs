use stardust_xr_molecules::{fusion::data::PulseReceiverHandler, keyboard::KeyboardEvent};
use std::char;
use std::collections::HashMap;
use xkbcommon::xkb::{self, Context, Keymap, State, FORMAT_TEXT_V1, KEYMAP_COMPILE_NO_FLAGS};

pub struct KeyboardHandler {
	keyboard_states: Vec<i32>,
	keyboard_function_map: HashMap<Vec<i32>, fn(state: &State)>,
	keyboard_on_utf_key_typing: fn(buf: &str),
	every_press: fn(),
	state: State,
	context: Context,
	keymap: Keymap,
}

impl KeyboardHandler {
	pub fn new() -> Self {
		let keymap = Keymap::new_from_names(
			&Context::new(xkb::CONTEXT_NO_FLAGS),
			"",
			"",
			"",
			"",
			None,
			xkb::KEYMAP_FORMAT_TEXT_V1,
		)
		.unwrap();
		KeyboardHandler {
			keyboard_states: Vec::new(),
			keyboard_function_map: HashMap::new(),
			keyboard_on_utf_key_typing: |_| {},
			every_press: || {},
			state: State::new(&keymap),
			context: Context::new(xkb::CONTEXT_NO_FLAGS),
			keymap,
		}
	}

	pub fn set_keymap(&mut self, keymap: String) {
		self.keymap = Keymap::new_from_string(
			&self.context,
			keymap,
			FORMAT_TEXT_V1,
			KEYMAP_COMPILE_NO_FLAGS,
		)
		.unwrap();
		self.state = State::new(&self.keymap);
	}

	pub fn run_functions(&self) -> bool {
		let mut matched = false;
		permutation(self.keyboard_states.clone(), Vec::new(), &mut |v2| {
			if let Some(func) = self.keyboard_function_map.get(&v2) {
				matched = true;
				func(&self.state);
			}
		});
		matched
	}

	pub fn update_key_press(&mut self, scancode: u32, remove_key: bool) {
		let mybuf = self
			.state
			.key_get_syms(scancode)
			.iter()
			.map(|keysym| *keysym as i32)
			.collect::<Vec<i32>>();
		if remove_key {
			self.keyboard_states = self
				.keyboard_states
				.clone()
				.into_iter()
				.filter(|key| !mybuf.contains(key))
				.collect();
		} else {
			self.keyboard_states.extend(mybuf.into_iter());
			if !self.run_functions() {
				let buf = self.state.key_get_utf32(scancode);
				// Convert the `u32` slice to a `String` using `from_u32_slice`
				let utf32_string = char::from_u32(buf).unwrap().to_string();
				(self.keyboard_on_utf_key_typing)(&utf32_string);
			}
		}
		(self.every_press)();
	}
}
impl PulseReceiverHandler for KeyboardHandler {
	fn data(&mut self, _uid: &str, data: &[u8], _data_reader: flexbuffers::MapReader<&[u8]>) {
		let key_event = KeyboardEvent::from_pulse_data(data).unwrap();
		key_event.update_xkb_state(&mut self.state);
	}
}
unsafe impl Send for KeyboardHandler {}
unsafe impl Sync for KeyboardHandler {}

pub fn permutation<F: FnMut(Vec<i32>)>(v: Vec<i32>, mut v2: Vec<i32>, function: &mut F) {
	let mut v_copy = v.clone();
	if v.is_empty() {
		function(v2);
	} else {
		if let Some(last) = v_copy.pop() {
			permutation(v_copy.clone(), v2.clone(), function);
			v2.push(last);
			permutation(v_copy, v2, function);
		}
	}
}
