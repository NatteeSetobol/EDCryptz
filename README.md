
```
.,:::::::::::::-.    .,-::::: :::::::...-:.     ::-.::::::::::. :::::::::::::::::::::
;;;;'''' ;;,   `';,,;;;'````' ;;;;``;;;;';;.   ;;;;' `;;;```.;;;;;;;;;;;'''''`````;;;
[[cccc  `[[     [[[[[         [[[,/[[['  '[[,[[['    `]]nnn]]'      [[         .n[['
$$""""   $$,    $$$$$         $$$$$$c      c$$"       $$$""         $$       ,$$P
888oo,__ 888_,o8P'`88bo,__,o, 888b "88bo,,8P"`        888o          88,    ,888bo,_ 
""""YUMMMMMMMP"`    "YUMMMMMP"MMMM   "W"mM"           YMMMb         MMM     `""*UMM
```
### BUILDING:
```
git clone https://github.com/NatteeSetobol/EDCryptz
cd EDcryptz
./build.sh
```
Once it's done, the execute should be in the build folder


### USING EDCryptz
Using EDCryptz is simple. To view options use -h. Here are some examples:


## Encrypt File/Directory Example
```
./build/decryptz -f file_or_directory_to_encrypt -e -o out_file_or_directoy
```

Once you have done that, you will get a key that looks something like this,

```
AA BB CC DD EE FF 00 01 02 03 04 05 06 07 08 09
```

Be sure to save your key somewhere safe


## Decrypt File/Directory Example

```
./build/Decryptz -f encrypted_directory_or_file -d -o output_normal_file -k "AABBCCDDEEFF00010203040506070809"
```

Note that I have erased all the spaces and left no spaces for the key. 
