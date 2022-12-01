use mint::Vector3;
use stardust_xr_molecules::fusion::drawable::Text;

pub struct Entry {
	pub text: Text,
	pub text_string: String,
	pub selected: bool,
	pub origin: Vector3<f32>,
	pub offset_origin: Vector3<f32>,
}

impl Entry {
	pub fn do_something_to_text<F: FnOnce(&mut String)>(&mut self, func: F) {
		func(&mut self.text_string);
		self.text.set_text(&self.text_string).unwrap();
	}

	pub fn set_unselected(&mut self) {
		self.selected = false;
		self.do_something_to_text(|string| {
			string.drain(0..1);
		});
		self.text.set_position(None, self.origin).unwrap();
	}

	pub fn is_selected(&self) -> bool {
		self.selected
	}

	pub fn set_selected(&mut self) {
		self.selected = true;
		self.do_something_to_text(|string| {
			string.insert(0, '>');
		});
		self.text.set_position(None, self.offset_origin).unwrap();
	}

	pub fn get_text(&self) -> &Text {
		&self.text
	}

	pub fn get_string(&self) -> &str {
		&self.text_string
	}
}
