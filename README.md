# xen-pv-console(write only)

write only な pv console を実装した単純なカーネル。  
世界時をコンソールに出力する。

下記例:

```shell
[ell@centos ~]$ make
[ell@centos ~]$ sudo xl create -c domain_config 
[ell@centos ~]$ sudo xl create -c domain_config 
Parsing config from domain_config
Hello from printk!
2019/10/15 09:57:07
```