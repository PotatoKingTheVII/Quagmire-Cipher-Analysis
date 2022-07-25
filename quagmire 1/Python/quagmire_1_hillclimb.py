import random
import math

#Vigenere decode function
def vigDecode(ciphertext, key):
    alphabet = "abcdefghijklmnopqrstuvwxyz"
    alphabetU = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    alph_dict = {"a":0, "b":1, "c":2, "d":3, "e":4, "f":5, "g":6, "h":7, "i":8, "j":9, "k":10,
                 "l":11, "m":12, "n":13, "o":14, "p":15, "q":16, "r":17, "s":18, "t":19, "u":20,
                 "v":21, "w":22, "x":23, "y":24, "z":25}
    
    alph_dict_U = {"A":0, "B":1, "C":2, "D":3, "E":4, "F":5, "G":6, "H":7, "I":8, "J":9, "K":10,
                 "L":11, "M":12, "N":13, "O":14, "P":15, "Q":16, "R":17, "S":18, "T":19, "U":20,
                 "V":21, "W":22, "X":23, "Y":24, "Z":25}

    #Convert the key to numerical shifts:
    keyShifts = []
    for i, letter in enumerate(key):
        keyShifts.append(alphabet.find(letter))
    key_length = len(key)

    #Do the actual decryption:
    plaintext = ""
    shiftCount = 0

    for i in range(0, len(ciphertext)):
        shift = 0
        letter = ciphertext[i]

        plaintext+=alphabet[(alph_dict[letter] - keyShifts[shiftCount%key_length]) % 26]
        shiftCount+=1

    return plaintext

def sub(ct, ct_alph):
    """Return the plaintext after subbing the provided
       ct alph
    """
    alph = "abcdefghijklmnopqrstuvwxyz"

    plaintext = ct
    for i, letter in enumerate(alph):
        plaintext = plaintext.replace(letter, ct_alph[i].upper())
    plaintext = plaintext.lower()

    return plaintext

#Credit to http://practicalcryptography.com/cryptanalysis/text-characterisation/quadgrams/ for the method+quadgrams
def fitnessQuadgram(text, quadGrams, min_val):
    #If our chunk isn't in the list then add this for a minimum
    fitness = 0

    #Loop through the text incrementing one at a time
    #For overlapping quadgrams
    for i in range(0,len(text)-3):
        current_chunk = text[i:i+4]
        if current_chunk in quadGrams:
            fitness+=quadGrams[current_chunk]
        else:
            fitness+= min_val

    return fitness


def mutate(alph_in):
    #Pick 2 random letters and swap them. Keep trying till get 2 unique numbers
    n1 = random.randint(0, 25)
    n2 = random.randint(0, 25)
    
    while n1 == n2:
        n1 = random.randint(0, 25)
        n2 = random.randint(0, 25)
    
    mutated_alph = list(alph_in)
    mutated_alph[n1] = mutated_alph[n2]
    mutated_alph[n2] = alph_in[n1]
    mutated_alph = "".join(mutated_alph)
    
    return mutated_alph


def mutate_vig(key):
    alph = "abcdefghijklmnopqrstuvwxyz"
    #Pick 2 random letters and swap them. Keep trying till get 2 unique numbers
    n1 = random.randint(0, 25)
    n2 = random.randint(0, len(key)-1)

    tmp_key = list(key)
    tmp_key[n2] = alph[n1]

    return "".join(tmp_key)


####################USER INPUTS####################
key_length = 4
ct = """NAOZCBKYMPYNVGTYZSXVNSCZSYFLNPKZYPWRCCKDYKGNJYNRROWZFGPORYNVRPYSVUKEDSKYMOMCRPYZDHMVVPYMRPKEDDMEVWDEVCNZQUYSSPWRMNYFYBDXRZQRZDOZFNCRSGKRDYXJHYIORAMFDCNRROPZYPONCCGXNPWFWYXDEIKYUWCLWLYDDWNVRUQVVKGOVYOUJCUVZSNVNATJRGUZHZUNPFIRVGYHOGMPUKYXOGMPUYXQJCUVZSUREOCXNFQLEQZNVWDEVCNNCNIOCCMUSYCELOBRNBIZZHKYMIMYMKXNDCOZYBWRVGYONNBJNBMWZYXJZQNRVWQGRPWLDWCXNLZRYWXMLAMYEWUVAWOUJKEFAGYZUWXMDIMCAQCREIQDEYKCDUYFCDMEXCBANWPMZKPWZJQJSKBEXKCRDKBCJWVRIAKERBWRCCCEXCLCNBENECGRCCFRCUZCZQPZSUYFDKXNACBSRADCRLYCEAMCQYUVOOFRCUZCZQPWNWLZEYDXLJLMZWXMXCBRJKEMZPULYPYYJKECSQJKZSDXNPCWRSMGRPYFDSQVVZQLYNYHOUQONNBJLPYVQUYFDPYASGILYXKYEIQXZQCRXCINQYVXRUNNCNILDPWNEDEKKXQVNGKEEGQDACOLNGPNJXBNQQMELKXYRRQCEIYFMIDLQHMURWDEXNQRQYIDMNMQROOXZKUEXNQRQYIDXWLXDAWZZGDXZOQHRNQNHFGNCBDXCCQQNUCPZGURMCKWMGMQLPYZUYPNJYXQXWDPXIKURBMCZQXQEIQXLRQJZQPLQAYWRZMPUBKSSCBRYPWLOYBCJYBELCLCZSKYMYVFDPMPXCUZZFCMZKPXRYBNOKEESNMYUWQJRYWJZQLZLJLEZPWRSQXRCYUYZWVYZPLZLJLRGCBJOKPJUJYHDODLYXCZWCYYRUYFQWQQZJDHNODRLPYYNOAFLNBRVOEPXYWZEIQNQWLFROCXRAYFVBWNGCSFDPLZEPQYZQDZSPWRHYI"""
###################################################


ct = ct.lower() 
ct = "".join(filter(str.isalpha, ct))
#Load the expected percentages from a reference file
with open("quadgrams.txt", "r") as fin:
    lines = fin.readlines()
    quadGrams = {}  #Lookup for percentages
    for line in lines:
        content = line.split(" ")
        quadGrams[content[0]] = int(content[1])

    total_count = sum(quadGrams.values())

    #Convert to log10 percentages
    for quadGram in quadGrams.keys():
        quadGrams[quadGram] = math.log10(float(quadGrams[quadGram]/total_count))

    #Calculate minimum value for quads not in our list
    min_val = math.log10(0.01/total_count)

best_top_fitness = -1e10
best_top_key = "e"*key_length   #e is chosen as the most likely letter to be in the key so a decent starting place

while True:
    vig_trial_key = mutate_vig(best_top_key)
    trial_vig_pt = vigDecode(ct, vig_trial_key)

    #Reset sub iteration counts for the monoalph
    iteration_count_limit = 2500   #How long should we search for? 2500-3000 usually works well
    iteration_count = 0
    best_fitness = -1e10
    trial_alph = "abcdefghijklmnopqrstuvwxyz"
    best_alph = trial_alph

    while iteration_count < iteration_count_limit:
        
        trial_alph = mutate(best_alph)
        trial_pt = sub(trial_vig_pt, trial_alph)
        trial_score = fitnessQuadgram(trial_pt.upper(), quadGrams, min_val)

        #If this was better than the previous best set it to the new best values
        if(trial_score > best_fitness):
            best_fitness = trial_score
            best_alph = trial_alph
            best_trial_pt = trial_pt

        iteration_count+=1

    #Now we check if after optimising it monoalph if this vig key was better overall
    if(best_fitness > best_top_fitness):
        best_top_fitness = best_fitness
        best_top_key = vig_trial_key
        #print(trial_pt, best_fitness, iteration_count, best_alph, vig_trial_key)
        print("Best fitness:", best_fitness, " Using key and alphabet:", best_top_key, "||", best_alph, "\n", best_trial_pt, "\n")
