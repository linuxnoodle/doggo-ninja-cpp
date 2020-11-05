#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <curl/curl.h>

using namespace std;

// Basic memory struct for storing memory.
struct memory {
    char *memory;
    size_t size;
};

// Function to save response from doggo.ninja.
static size_t writeCallBack(void *contents, size_t size, size_t nmemb, void *userp){
    size_t realSize = size * nmemb;
    struct memory *mem = (struct memory*)userp;

    char *pointer = realloc(mem->memory, mem->size + realSize + 1);
    if (pointer == NULL) {
        cout << "Not enough memory left. (realloc returned NULL)" << endl;
        return 1;
    }

    mem->memory = pointer;
    memcpy(&(mem->memory[mem->size]), contents, realSize);
    mem->size += realSize;
    mem->memory[mem->size] = 0;

    return realSize;
}

// Checks whether a token is valid or not.
bool checkTokenValid(string token){
    CURL* curl;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, "https://pat.doggo.ninja/v1/me");
        
        struct memory chunk;
        chunk.memory = NULL;
        chunk.size = 0;

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("authorization: Bearer " + token).c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode response = curl_easy_perform(curl);
        
        if (response != CURLE_OK){
            cerr << "curl_easy_perform() failed:\n" << curl_easy_strerror(response) << endl;
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers); 
            return false;
        } else {
            if (strcmp(chunk.memory, "{\"statusCode\":401,\"error\":\"Unauthorized\",\"message\":\"Invalid token\"}") == 0){
                return false;
            } else {
                return true;
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return false;
}

void uploadFile(string token, string path){
    if (!checkTokenValid(token)){
        cerr << "ERROR: Token in Tokenfile is invalid. Please save your token through \"./doggo-ninja.exe -s [token]\"." << endl;
        return;
    }

    ifstream file;
    file.open(path, fstream::in);
    string data = "";

    if (file.is_open()){
        string line;
        while (getline (file, line)){
            data += line + "\n";
        }
        file.close();
    } else {
        cerr << "ERROR: Provided with an invalid or unreadable file." << endl;
        return;
    }

    string fullFilename = path.substr(path.find_last_of("/\\") + 1);
    
    string url = "https://pat.doggo.ninja/v1/upload?originalName=" + fullFilename;

    CURL *curl;
    curl = curl_easy_init();
    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
        //https://pat.doggo.ninja/v1/upload?originalName=$basename&mimeType=$mime_type
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());

        curl_easy_perform(curl);
        cout << "\nFinished uploading." << endl;

        curl_easy_cleanup(curl);
    }
}

int main(int argc, char *argv[]){
    if (argc == 1 || strcmp(argv[1], "-h") == 0){
        #ifdef _WIN32
            cout << "Usage:\n" << "\t./doggo-ninja.exe -h\t\tBrings up this screen.\n" << "\t./doggo-ninja.exe -t [token]\tSaves your token to a file.\n" << "\t./doggo.ninja.exe -u [filename]\tUploads a file.\n" << endl;
        #else
            cout << "Usage:\n" << "\t./doggo-ninja -h\t\tBrings up this screen.\n" << "\t./doggo-ninja -t [token]\tSaves your token to a file.\n" << "\t./doggo.ninja -u [filename]\tUploads a file.\n" << endl;
        #endif
        return 0;
    } else if (strcmp(argv[1], "-t") == 0){
        if (argc == 3){
            bool tokenIsValid = checkTokenValid(argv[2]);
            if (tokenIsValid){
                fstream tokenFile;
                #ifdef _WIN32
                    tokenFile.open(strcat(getenv("APPDATA"), "token.txt"), fstream::out);
                    tokenFile << argv[2];
                    tokenFile.close();

                    cout << "Token written to file at" << strcat(getenv("APPDATA"), "\\doggo-token.txt") << endl;
                #else
                    tokenFile.open(strcat(getenv("HOME"), "/.config/doggo-token.txt"), fstream::out);
                    tokenFile << argv[2];
                    tokenFile.close();

                    cout << "Token written to file at " << "~/.config/doggo-token.txt" << endl;
                #endif
            } else {
                cerr << "ERROR: Token not valid." << endl;
            }
        } else {
            cerr << "ERROR: Too many arguments provided." << endl;
        }
   } else if (strcmp(argv[1], "-u") == 0){
        if (argc < 3){
            cerr << "ERROR: No path to a file to upload was provided." << endl;
            return -1;
        }
        fstream tokenFile;
        string token;
        if (tokenFile){
            #ifdef _WIN32
                tokenFile.open(strcat(getenv("APPDATA"), "token.txt"), fstream::in);
            #else
                tokenFile.open(strcat(getenv("HOME"), "/.local/share/token.txt"), fstream::in);
            #endif

            if (tokenFile){
                getline(tokenFile, token);
                tokenFile.close();

                uploadFile(token, argv[2]);
            } else {
                cerr << "ERROR: Tokenfile not found. Please save your token through \"./doggo-ninja -s [token]\"." << endl;
            }
        } else {
            cerr << "ERROR: Token not supplied." << endl;
        }
        return 0;
   } else {
        cerr << "ERROR: Flag " << argv[1] << " does not exist.\n" << "Usage:\n" << "\t./doggo-ninja -h\t\tBrings up this screen.\n" << "\t./doggo-ninja -s [token]\tSaves your token to a file.\n" << "\t./doggo.ninja -u [filename]\tUploads a file.\n" << endl;
        return 0;
   }
}