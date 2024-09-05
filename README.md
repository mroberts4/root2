
# 
Protect Data Confidentiality for On-device Machine Learning through Split Mode

The following are the folder descriptions and description of their contents:

-   eapp folder: contains the files for the implementation of MobileNet and the three   enclaves that run the three parts of the MobileNet CNN. 
-   host folder: **contains the files for a host that will launch the three enclaves, perform remote attestation, and send image data to the MobileNet enclaves. 
-   Both folders also include files necessary for AES-CBC128 encryption, which is used for the encryption and decryption of transmitted data. Also included is the CMakeLists file for building this Keystone package.
