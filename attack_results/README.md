# SideLine - Zynq SCA results

This folder contain SCA attack scripts for AES and RSA and the experimental results that were obtained on the Zybo board.
- To perform a correlation power analysis on openSSL AES use the AES_SCA.py script located in [AES_results]
- To perform a simple power analysis attack on RSA use the RSA_SCA.py script located in [RSA_results]
## OpenSSL AES

- Attack results on AES OpenSSL implementation

<p align="center">
<img src="https://user-images.githubusercontent.com/67143135/88479697-424f1100-cf51-11ea-8a23-002140716dde.png" width="600" height="370">
</p>

## Custom Naive RSA

- Attack results on naive RSA implementation

<p align="center">
<img src="https://user-images.githubusercontent.com/67143135/91450799-fc050e80-e87c-11ea-8802-e4068d0c06e4.png" width="700" height="250">
</p>

## Custom Constant time RSA

- Attack results: custom Constant Time RSA implementation

<p align="center">
<img src="https://user-images.githubusercontent.com/67143135/91450793-fad3e180-e87c-11ea-93f8-aa620d8efda4.png" width="700" height="250">
</p>

## Custom RSA Ladder

- Attack results: Montgomery Ladder RSA implementation

<p align="center">
<img src="https://user-images.githubusercontent.com/67143135/91450797-fb6c7800-e87c-11ea-8685-8dd261664c61.png" width="700" height="250">
</p>

## Full RSA results
Full acquisition of Naive, Constant time and Ladder RSA are available in binary format [full_RSA]


