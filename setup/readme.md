
# Setup

* Install prerequisites for visual studio remote linux
* Install cudnn:
	* Download cudnn .tgz file
	* Copy it to the VM
	
		scp -i .\cybera_test_keypair.pem .\cudnn-10.1-linux-x64-v7.6.5.32.tgz  ubuntu@162.246.157.179:/home/ubuntu/cudnn-10.1-linux-x64-v7.6.5.32.tgz

	* Untar:
	
		tar -zxvf cudnn-10.1-linux-x64-v7.6.5.32.tgz
	
	* Copy to system folders and change permissions:

		sudo cp cuda/include/cudnn.h /usr/local/cuda/include
		sudo cp cuda/lib64/libcudnn* /usr/local/cuda/lib64
		sudo chmod a+r /usr/local/cuda/include/cudnn.h /usr/local/cuda/lib64/libcudnn*

	* Setup envvars:

		echo "export CUDA_HOME=/usr/local/cuda" >> ~/.bashrc
		export CUDA_HOME=/usr/local/cuda
		echo "export CUDA_ROOT=/usr/local/cuda" >> ~/.bashrc
		export CUDA_ROOT=/usr/local/cuda
		echo "export PATH=$PATH:$CUDA_ROOT/bin" >> ~/.bashrc
		echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CUDA_ROOT/lib64" >> ~/.bashrc
		source /home/ubuntu/.bashrc

	* Install CUDA:
	
		wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/cuda-ubuntu1804.pin
		sudo mv cuda-ubuntu1804.pin /etc/apt/preferences.d/cuda-repository-pin-600
		sudo apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/7fa2af80.pub
		sudo add-apt-repository "deb http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/ /"
		sudo apt-get updatesudo apt-get -y install cuda-10-1

	* Download MNIST (for example)

		wget https://gist.githubusercontent.com/goldsborough/6dd52a5e01ed73a642c1e772084bcd03/raw/690b3760c735c7b5f6f95e29ea0f80bf0896f81d/download_mnist.py
		python3 download_mnist.py -d ~/.mnist/

    * Format volume
        
        sudo mkfs.ext4 /dev/sdc

    * Setup volume
        
        sudo mkdir /mnt/data
        sudo mount /dev/sdc /mnt/data
        sudo chown ubuntu:ubuntu /mnt/data
        
        