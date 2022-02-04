objcopy -I binary -O pe-x86-64 -B i386 cacert.pem ../cacert.o
objcopy -I binary -O pe-x86-64 -B i386 THIRD_PARTY_LICENCE ../third_party_licence.o
objcopy -I binary -O pe-x86-64 -B i386 resource_list.json ../resource_list.o
objcopy -I binary -O pe-x86-64 -B i386 java_details.jar ../java_details.o
objcopy -I binary -O pe-x86-64 -B i386 7z_32.dll ../7z_32.o
objcopy -I binary -O pe-x86-64 -B i386 7z_64.dll ../7z_64.o