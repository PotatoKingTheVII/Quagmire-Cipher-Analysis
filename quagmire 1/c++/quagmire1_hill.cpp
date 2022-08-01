#include <iostream> //cout
#include <algorithm> //.replace in monoalph sub
#include <string> //strings
#include <fstream> //file io
#include <ctime> //setting rand seed
#include <cmath> //log10
#include "robin_hood.h" //faster unordered_maps https://github.com/martinus/robin-hood-hashing

using namespace std;    //forgive me

//Load quadgrams from "quadgrams.txt" in the working directory into an unordered_map e.g. {"THEI", 1241251}
//return this and the value to use if a quadgram is not in the list used
tuple<robin_hood::unordered_map<string, double>, double> load_quadgrams()
{
    robin_hood::unordered_map<string, double> quadgram_map;
    string line;
    ifstream wordlist_file;
    wordlist_file.open("quadgrams.txt");

    if (!wordlist_file.is_open())
    {
        perror("Error opening quadgrams.txt file");
        exit(EXIT_FAILURE);
    }

    //First find the total count to use for the next proper loop
    unsigned long long int total_count = 0;
    while (getline(wordlist_file, line))
    {
        double current_count = stoull(line.substr(5, line.length()));
        total_count += current_count;
    }

    //Seek back to start of file for next reading
    wordlist_file.clear();
    wordlist_file.seekg(0);

    //Set the normalised log10 values for the quadgram unordered_map
    while (getline(wordlist_file, line))
    {
        string current_quadgram = line.substr(0, 4);
        double current_count = std::stoull(line.substr(5));
        quadgram_map[current_quadgram] = log10(current_count / total_count);
    }
    wordlist_file.close();

    //Find the min value to be used in lieu of a quadgram and return tuple to hold both it and the robin_hood::unordered_map
    double min_value = log10(0.01 / total_count);
    tuple<robin_hood::unordered_map<string, double>, double> quadgram_info = make_tuple(quadgram_map, min_value);

    return quadgram_info;
}

//Return a fitness score for the given text. Closer to 0 = higher score
//Takes uppercase alpha string with no spacing
double quadgram_fitness(string &ciphertext, robin_hood::unordered_map<string, double> &quadgram_map, double &min_value)
{
    double fitness = 0;

    for (int i = 0; i < ciphertext.length() - 3; i++)
    {
        string current_chunk = ciphertext.substr(i, 4);
        double chunk_score = quadgram_map[current_chunk];

        if (chunk_score == 0)   //If we don't have this quadgram in the unordered_mapping 
        {
            chunk_score = min_value;
        }
        fitness += chunk_score;
    }

    return fitness;
}

//Return the input string (uppercase input) after substituting the provided alphabet (lowercase input)
//case difference is to avoid subbing in previously subbed letters more than once
string monoalph_sub(string ciphertext, string &ct_alphabet)
{
    const string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < alphabet.length(); i++)
    {
        replace(ciphertext.begin(), ciphertext.end(), alphabet[i], ct_alphabet[i]); //Replace first arg with second
    }

    transform(ciphertext.begin(), ciphertext.end(), ciphertext.begin(), ::toupper); //Back to uppercase to match what our input was given as
    return ciphertext;
}

//Randomly mutate the provided alphabet swapping 2 letters
string mutate_alph(string ct_alpha)
{
    int n1 = rand() % 26;   //Note modulo bias might favour certain letter swaps here
    int n2 = rand() % 26;

    //Make sure they're not equal
    while (n1 == n2)
    {
        n1 = rand() % 26;
        n2 = rand() % 26;
    }

    string new_ct_alpha = ct_alpha;
    new_ct_alpha[n2] = ct_alpha[n1];
    new_ct_alpha[n1] = ct_alpha[n2];

    return new_ct_alpha;
}

//Randomly mutate the provided alphabet swapping 2 letters
string mutate_key(string key)
{
    const string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int n1 = rand() % 26;   //Note modulo bias might favour certain letter swaps here
    int n2 = rand() % (key.length());

    key[n2] = alphabet[n1];

    return key;
}

//Mod with expected negative behaviour
int mod(int x, int m) 
{
    return (x % m + m) % m;
}

//Input as uppercase string, return input vigenere decoded with key (Also uppercase)
string vigenere_decode(string &ciphertext, string &key)
{
    const string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int key_length = key.length();

    //Do the actual decryption now
    string plaintext = "";
    
    for (int i = 0; i < ciphertext.length(); i++)
    {
        plaintext += alphabet[mod((int(ciphertext[i]) - 65 - int(key[i % key_length]) - 65 ), 26)];
    }

    return plaintext;
}


int main(int argc, char* argv[])
{
    //Parse arguments:
    //Set placeholder user inputs to see if they're actually changed later
    string ct = "PLACEHOLDER_CT";
    int vig_keylength = 1337;

     if (argc == 1)  //If no flags were given
    {
         cout << "\nCorrect usage:\n\n"
             "quagmire1_hill.exe -n integer -c ciphertext\n\n"
             "-n : what vig keylength to check\n"
             "-c : ciphertext to decode, uppercase letters only no spacing\n";
    }

    //For each argument given
    for (int i = 1; i < argc; ++i)
    {
        string currentArg(argv[i]);

        //If the current argument is actually a flag
        if (currentArg[0] == '-')
        {
            string currentNextArg(argv[i + 1]);
            switch (currentArg[1])  //Check which flag
            {

            case 'c':   //Set ciphertext
                ct = string(argv[i + 1]);
                break;

            case 'n':   //Set keylength
                vig_keylength = stoi(argv[i + 1]);
                break;

            default:
                cout << "\nCorrect usage:\n\n"
                    "quagmire1_hill.exe -n integer -c ciphertext\n\n"
                    "-n : what vig keylength to check\n"
                    "-c : ciphertext to decode, uppercase letters only no spacing\n";

            }
        }
    }

    //Sanity check to make sure the user actually input all the needed values:
    if (ct == "PLACEHOLDER_CT" || vig_keylength == 1337)
    {
        return 1;
    }

    //Parsing finished. Actual program ahead
    srand(time(NULL)); //Seed the RNG so we get different results each runtime

    //Load + unpack quadgram statistics
    tuple<robin_hood::unordered_map<string, double>, double> quadgram_info = load_quadgrams();
    double min_quadgram_value = get<1>(quadgram_info);
    robin_hood::unordered_map<string, double> quadgram_map = get<0>(quadgram_info);

    //========================Hil climb section begin================================    
    //Outer / top hill climbing section
    double best_top_fitness = -1e10;
    string best_top_key(vig_keylength, 'E'); //Change length of this to bruteforce different key-lengths. "E" picked as most likely letter in key

    while (true)    //Run till user stops
    {
        //Mutate our vigenere key
        string vig_trial_key = mutate_key(best_top_key);
        string trial_vig_pt = vigenere_decode(ct, vig_trial_key);

        //Reset the monoalph hill climber settings
        int iteration_limit = 3000;
        int iteration_count = 0;
        double best_fitness = -1e10;
        string trial_alph = "abcdefghijklmnopqrstuvwxyz";
        string best_alph = trial_alph;
        string best_trial_pt;

        while (iteration_count < iteration_limit)   //Monoalph hill climber loop
        {
            //Mutate monoalph alphabet
            trial_alph = mutate_alph(best_alph);
            string trial_pt = monoalph_sub(trial_vig_pt, trial_alph);
            double trial_score = quadgram_fitness(trial_pt, quadgram_map, min_quadgram_value);

            //If this was better than the previous best set it to the new best values
            if (trial_score > best_fitness)
            {
                best_fitness = trial_score;
                best_alph = trial_alph;
                best_trial_pt = trial_pt;
            }

            iteration_count += 1;
        }

        //Was this vig key along with the best monoalph result for it better than our previous best?
        //If so set it as the new best key and its fitness as the new goal to beat
        if (best_fitness > best_top_fitness)
        {
            best_top_fitness = best_fitness;
            best_top_key = vig_trial_key;
            cout << "Best fitness: " << best_fitness << " Using key + alphabet: " << best_top_key << "  ||  " << best_alph << "\n" << best_trial_pt << "\n\n";
        }
    }
}
