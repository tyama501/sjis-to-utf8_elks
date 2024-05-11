# sjis-to-utf8 for ELKS

dolphilia様が以下で公開されているsjis-to-utf8を  
ELKS向けに改変したものです。公開ありがとうございます。  
オリジナル : https://github.com/dolphilia/sjis-to-utf8

Embeddable Linux Kernel Subset (ELKS)は以下で開発されている8086等Intel IA16アーキテクチャ向けのLinux-LikeなOSです。
https://github.com/ghaerr/elks

# How to use
./sjisutf8 sjisfile > utf8file  
(/bin/にインストールした場合はsjisutf8 sjisfile > utf8file)  

最新のnxjtxtvにpipeで繋いで表示することもメモリが足りればできます。  
sjisutf8 sjisfile | nxjtxtv  
https://github.com/tyama501/nxjtxtv

# Build
ELKSのBuild環境が必要です。  
ELKSのBuild環境構築後、elks/elkscmd/にsjis-to-utf8_elksを置き  
elks/elkscmd/sjis-to-utf8_elks/でmakeを行うとsjisutf8ができます。  


