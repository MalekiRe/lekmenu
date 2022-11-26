use std::sync::Arc;

use anyhow::Result;
use manifest_dir_macros::directory_relative_path;
use stardust_xr_molecules::{
	fusion::{
		client::{Client, LifeCycleHandler, LogicStepInfo},
		data::{PulseReceiver, PulseReceiverHandler},
		drawable::{Text, TextStyle},
		fields::SphereField,
		node::NodeError,
		resource::NamespacedResource,
	},
	keyboard::KEYBOARD_MASK,
};

#[tokio::main(flavor = "current_thread")]
async fn main() -> Result<()> {
	let (client, event_loop) = Client::connect_with_async_loop().await?;
	client.set_base_prefixes(&[directory_relative_path!("res")]);

	let _wrapped_root = client.wrap_root(LekMenu::new(&client)?);
	let field = SphereField::builder()
		.spatial_parent(client.get_root())
		.radius(0.1)
		.build()
		.unwrap();
	let _pulse_reciver = PulseReceiver::create(
		client.get_root(),
		None,
		None,
		&field,
		KEYBOARD_MASK.clone(),
		|_, _| PulseReceiverTest(client.clone()),
	)
	.unwrap();
	tokio::select! {
		_ = tokio::signal::ctrl_c() => Ok(()),
		_ = event_loop => Err(anyhow::anyhow!("Server crashed")),
	}
	//Ok(())
}

struct LekMenu {
	text: Text,
}
impl LekMenu {
	fn new(client: &Client) -> Result<Self, NodeError> {
		let mut style: TextStyle<NamespacedResource> = TextStyle::default();
		style.font_resource = Some(NamespacedResource::new("fonts", "common_case.ttf"));
		let text = Text::builder()
			.spatial_parent(client.get_root())
			.text_string("Testing")
			.style(style)
			.build()?;
		text.set_character_height(0.05)?;
		Ok(Self { text })
	}
}
impl LifeCycleHandler for LekMenu {
	fn logic_step(&mut self, _info: LogicStepInfo) {

		//none
	}
}
struct PulseReceiverTest(Arc<Client>);
impl PulseReceiverHandler for PulseReceiverTest {
	fn data(&mut self, uid: &str, data: &[u8], _data_reader: flexbuffers::MapReader<&[u8]>) {
		println!(
			"Pulse sender {} sent {}",
			uid,
			flexbuffers::Reader::get_root(data).unwrap()
		);
		self.0.stop_loop();
	}
}
//use resource::NamespacedResource;
// let (client, _event_loop) = client::Client::connect_with_async_loop().await?;
// client.set_base_prefixes(&[manifest_dir_macros::directory_relative_path!("res")]);
//
// let mut style: TextStyle<NamespacedResource> = TextStyle::default();
// style.font_resource = Some(NamespacedResource::new("fonts", "common_case.ttf"));
//
// let text = Text::builder()
//     .spatial_parent(client.get_root())
//     .text_string("Test Text")
//     .style(style)
//     .build()?;
//
// text.set_character_height(0.5)?;
// text.set_text("Test Text: Changed")?;
// println!("did the things");
// tokio::time::sleep(core::time::Duration::from_secs(60)).await;
