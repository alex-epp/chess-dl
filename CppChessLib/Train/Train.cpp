// Train.cpp : Defines the entry point for the application.
//

#include <torch/torch.h>
#include <iostream>

using namespace torch;


auto make_generator(int64_t k_noise_size) {
	return nn::Sequential(
		nn::Conv2d(nn::Conv2dOptions(k_noise_size, 256, 4).bias(false).transposed(true)),
		nn::BatchNorm(256),
		nn::Functional(torch::relu),

		nn::ConvTranspose2d(nn::ConvTranspose2dOptions(256, 128, 3).stride(2).padding(1).bias(false)),
		nn::BatchNorm(128),
		nn::Functional(torch::relu),

		nn::ConvTranspose2d(nn::ConvTranspose2dOptions(128, 64, 4).stride(2).padding(1).bias(false)),
		nn::BatchNorm(64),
		nn::Functional(torch::relu),

		nn::ConvTranspose2d(nn::ConvTranspose2dOptions(64, 1, 4).stride(2).padding(1).bias(false)),
		nn::Functional(torch::tanh)
	);
}

auto make_discriminator() {
	return nn::Sequential(
		nn::Conv2d(nn::Conv2dOptions(1, 64, 4).stride(2).padding(1).bias(false)),
		nn::Functional(torch::leaky_relu, 0.2),

		nn::Conv2d(nn::Conv2dOptions(64, 128, 4).stride(2).padding(1).bias(false)),
		nn::BatchNorm(128),
		nn::Functional(torch::leaky_relu, 0.2),

		nn::Conv2d(nn::Conv2dOptions(128, 256, 4).stride(2).padding(1).bias(false)),
		nn::BatchNorm(256),
		nn::Functional(torch::leaky_relu, 0.2),

		nn::Conv2d(nn::Conv2dOptions(256, 1, 3).stride(1).padding(0).bias(false)),
		nn::Functional(torch::sigmoid)
	);
}


int main()
{
	constexpr auto NOISE_SIZE = 100;
	constexpr auto BATCH_SIZE = 64;
	constexpr auto N_EPOCHS = 10;
	constexpr auto BATCHES_PER_EPOCH = 10;

	auto dataset = torch::data::datasets::MNIST("/home/ubuntu/.mnist/")
		.map(torch::data::transforms::Normalize<>(0.5, 0.5))
		.map(torch::data::transforms::Stack<>());
	auto data_loader = torch::data::make_data_loader(
		std::move(dataset),
		torch::data::DataLoaderOptions().batch_size(BATCH_SIZE).workers(8));

	auto generator = make_generator(NOISE_SIZE);
	auto discriminator = make_discriminator();

	torch::optim::Adam generator_optimizer(
		generator->parameters(), torch::optim::AdamOptions(2e-4).beta1(0.5));
	torch::optim::Adam discriminator_optimizer(
		discriminator->parameters(), torch::optim::AdamOptions(5e-4).beta1(0.5));


	for (int64_t epoch = 1; epoch <= N_EPOCHS; ++epoch) {
		int64_t batch_index = 0;
		for (torch::data::Example<>& batch : *data_loader) {
			// Train discriminator with real images.
			discriminator->zero_grad();
			torch::Tensor real_images = batch.data;
			torch::Tensor real_labels = torch::empty(batch.data.size(0)).uniform_(0.8, 1.0);
			torch::Tensor real_output = discriminator->forward(real_images);
			torch::Tensor d_loss_real = torch::binary_cross_entropy(real_output, real_labels);
			d_loss_real.backward();

			// Train discriminator with fake images.
			torch::Tensor noise = torch::randn({ batch.data.size(0), NOISE_SIZE, 1, 1 });
			torch::Tensor fake_images = generator->forward(noise);
			torch::Tensor fake_labels = torch::zeros(batch.data.size(0));
			torch::Tensor fake_output = discriminator->forward(fake_images.detach());
			torch::Tensor d_loss_fake = torch::binary_cross_entropy(fake_output, fake_labels);
			d_loss_fake.backward();

			torch::Tensor d_loss = d_loss_real + d_loss_fake;
			discriminator_optimizer.step();

			// Train generator.
			generator->zero_grad();
			fake_labels.fill_(1);
			fake_output = discriminator->forward(fake_images);
			torch::Tensor g_loss = torch::binary_cross_entropy(fake_output, fake_labels);
			g_loss.backward();
			generator_optimizer.step();

			std::printf(
				"\r[%2ld/%2ld][%3ld/%3ld] D_loss: %.4f | G_loss: %.4f",
				epoch,
				N_EPOCHS,
				++batch_index,
				BATCHES_PER_EPOCH,
				d_loss.item<float>(),
				g_loss.item<float>());
		}
	}
	

	std::cin.get();
}
