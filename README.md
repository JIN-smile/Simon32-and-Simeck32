# Simon32-and-Simeck32
**`Title`**：Rotational-XOR Cryptanalysis of Simon-like Block Ciphers  
**`Author`**: Jinyu Lu, Yunwen Liu, Tomer Ashur, Bing Sun and Chao Li   
**`Abstract`**：Rotational-XOR cryptanalysis is a cryptanalytic method aimed at finding distinguishable statistical properties in ARX-C ciphers, *i.e.*, ciphers that can be described only by using modular addition, cyclic rotation, XOR, and the injection of constants. In this paper we extend RX-cryptanalysis to AND-RX ciphers, a similar design paradigm where the modular addition is replaced by vectorial bitwise AND; such ciphers include the block cipher families Simon and Simeck. We analyze the propagation of RX-differences through AND-RX rounds and develop closed form formula for their expected probability. Finally, we formulate an SMT model for searching RX-characteristics in simon and simeck. 
	
Evaluating our model we find RX-characteristics of up to 20, 27, and 35 rounds with respective probabilities of 2<sup>-26</sup>, 2<sup>-42</sup>, and 2<sup>-54</sup> for versions of simeck with block sizes of 32, 48, and 64 bits, respectively, for large classes of weak keys in the related-key model. In most cases, these are the longest published distinguishers for the respective variants of simeck. 

Interestingly, when we apply the model to the block cipher simon, the best characteristic we are able to find covers 11 rounds of SIMONnospace 32 with probability 2<sup>-24</sup>. To explain the gap between simon and simeck in terms of the number of distinguished rounds we study the impact of the key schedule and the specific rotation amounts of the round function on the propagation of RX-characteristics in Simon-like ciphers. 

