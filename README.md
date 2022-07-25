## Quagmire 1
A general hill climb method is used with an "inner" monoalph and outer vigenere loop. For each trial vigenere key the best possible monoalph plaintext is found with the inner substitution loop and set as the new goal to beat. Note that the resulting keys don't distinguish between any rotations/reversals (meaning to recover the actual key used first substitute the key with the CT alphabet and then check all rotations and atbash+rotations). To use the resulting key directly with other quagmire implementations set the indicator to the first letter of the cipher alphabet.

**Python Version:**

User inputs found within file to be edited.

**C++ Version:**

~~what the hell's a make file~~ Depends on https://github.com/martinus/robin-hood-hashing and tested on C++20 MSVC 19. Used as:
> quagmire1_hill.exe -n vig_key_length -c ciphertext 

 Note that both

## Quadgrams
All require quadgrams.txt which can be found at http://practicalcryptography.com/cryptanalysis/text-characterisation/quadgrams/
