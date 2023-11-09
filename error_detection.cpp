#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
//from https://stackoverflow.com/questions/13630002/storing-binary-number-in-integer-variable
#include<bitset>
#include <algorithm> 
using namespace std;

//input the two polynomials for both CRC-8 and CRC-16
bitset<9> poly8("100110001");
bitset<17> poly16("10001000000100001");

string crc8(bitset<72> input){
    //find max index
    int size = input.size() - 1;
    //set starting variables
    bitset<9> temp;
    bitset<9> zero("000000000");
    //input the first 9 values (since dividend only looks at 9 digits at a time)
    for(int i=9-1;i>=0;i--){
        if(input.test(size)){
            temp.set(i);
        }
        else{
            temp.reset(i);
        }
        size--;
    }
    //start logic of checking leftmost bit and either XOR with the polynomial or XOR with 0
    while(size>=0){
        if(temp.test(8)){
            temp ^= poly8;
            temp <<= 1;
            //pull down bit from the main dividend (after performing the XOR)
            if(input.test(size)){
                temp.set(0);
            }
            else{
                temp.reset(0);
            }
        }
        else{
            temp ^= zero;
            temp <<= 1;
            if(input.test(size)){
                temp.set(0);
            }
            else{
                temp.reset(0);
            }
        }
        size--;
    }
    
    //Cut last digit since CRC is only 8 digits
    return temp.to_string().substr(1, temp.to_string().size() - 1);;
}

//same exact logic
string crc16(bitset<80> input){
    int size = input.size() - 1;
    
    bitset<17> temp;
    bitset<17> zero("00000000000000000");
    for(int i=17-1;i>=0;i--){
        if(input.test(size)){
            temp.set(i);
        }
        else{
            temp.reset(i);
        }
        size--;
    }
    
    while(size>=0){
        if(temp.test(16)){
            temp ^= poly16;
            temp <<= 1;
            if(input.test(size)){
                temp.set(0);
            }
            else{
                temp.reset(0);
            }
        }
        else{
            temp ^= zero;
            temp <<= 1;
            if(input.test(size)){
                temp.set(0);
            }
            else{
                temp.reset(0);
            }
        }
        size--;
    }
    
    
    return temp.to_string().substr(1, temp.to_string().size() - 1);;
}
    
int checksum(string input){
    int sum = 0;
    //split the input into 8 bit numbers
    for(int i=0;i<8;i++){
        string sub = input.substr(i*8, (i+1)*8);
        sum += bitset<8>(sub).to_ulong();
    }
    //algorithm for checksum
    int q = sum/256;
    int r = sum%256;
    int csum;
    while(q+r > 256){
        csum = q+r;
        q = csum/256;
        r = csum%256;
    }
    csum = 255-(q+r);
    return csum;
}
//quick function to perform bitwise addition (0 or 1)
int addMod2(int a, int b){
    if(a+b>1){
        return 0;
    }
    return a+b;
}
//outputting the column and row parities
void print2DVector(const vector<vector<int>>& vec) {
    cout<<"Column Parity: ";
    for (int i = 0; i<vec[0].size(); i++)
    {
        cout << vec[0][i];
    }
    cout<<endl<<"Row Parity: ";
    for (int i = 0; i<vec[1].size(); i++)
    {
        cout << vec[1][i];
    }
    cout<<endl;
}

vector<vector<int>> Parity(string inputBinary){
    //set output variables
    vector<vector<int>> final;
    vector<int> temp;
    //put the 8 bit blocks into the vector
    for(int i = 0; i<8 ; i++){
        for(int j = i*8; j < (i+1)*8; j++){
            temp.push_back(inputBinary[j] - '0');
        }
        final.push_back(temp);
        temp.clear();
    }
    vector<int> sum1;
    vector<int> sum2(8, 0);
    //calculate column parity
    for(int i=0; i<final.size(); i++){
        for(int j=0;j<final[i].size();j++){
            sum2[j] = addMod2(sum2[j], final[i][j]);
        }
        
    }
    final.push_back(sum2);
    //calculate row parity (taking into account column parity)
    for(int i=0; i<final.size(); i++){
        for(int j=0; j<final[i].size()-1; j++){
            final[i][j+1] = addMod2(final[i][j+1], final[i][j]);
        }
        sum1.push_back(final[i][7]);
    }
    final.clear();
    
    final.push_back(sum2);
    final.push_back(sum1);

    return final;
}

string introduceError(string a, string b){
    //XOR the input and error (up to the length of the longer string)
    int size = max(a.size(),b.size());
    for(int i=0;i<size;i++){
        if((a[i] == '1' && b[i] == '0') || (a[i] == '0' && b[i] == '1')){
            a[i] = '1';
        }
        else{
            a[i] = '0';
        }
    }
    return a;
}
//logic to find crc and compare with error input crc
void crcCheck(string input, string inputError){
    string input8pad = input.append(8, '0');
    bitset<72> inputBit(input8pad);
    string test = crc8(inputBit);
    cout<<"CRC-8: "<<test<<endl;
    
    string input16pad = input.append(16, '0');
    bitset<80> inputBit2(input16pad);
    string test2 = crc16(inputBit2);
    cout<<"CRC-16: "<<test2<<endl;
    
    string input8padError = inputError.append(8, '0');
    bitset<72> inputBitError(input8pad);
    if(test == crc8(inputBitError)){
        cout<<"CRC-8 Result: Pass"<<endl;
    }
    else{
        cout<<"CRC-8 Result: Fail"<<endl;
    }
    string input16padError = inputError.append(16, '0');
    bitset<80> inputBit2Error(input16padError);
    if(test2 == crc16(inputBit2Error)){
        cout<<"CRC-16 Result: Pass"<<endl;
    }
    else{
        cout<<"CRC-16 Result: Fail"<<endl;
    }
    
}
//logic to find checksum and compare with error input checksum
void checksumCheck(string input, string inputError){
    string binary = bitset<8>(checksum(input)).to_string();
    cout<<"Checksum: "<< binary<<endl;
    string binaryError = bitset<8>(checksum(inputError)).to_string();
    if(binaryError == binary){
        cout<<"Result: Pass"<<endl;
    }
    else{
        cout<<"Result: Fail"<<endl;
    }
}
//logic to find parity and compare with error input parity
void parityCheck(string input, string inputError){
    vector<vector<int>> temp1 = Parity(input);
    print2DVector(temp1);
    vector<vector<int>> temp2 = Parity(inputError);
    if((temp1[0] == temp2[0]) && (temp1[1] == temp2[1])){
        cout<<"Result: Pass"<<endl;
    }
    else{
        cout<<"Result: Fail"<<endl;
    }
}

int main()
{
    std::string txtfile = "input.txt";
    std::ifstream inputFile(txtfile);
    
    if(!inputFile.is_open()){
        std::cout<<"Error: Could not open file!";
    }
    
    std::string line;
    while(std::getline(inputFile, line)){
    //get input from text file and seperate into input and error
    istringstream inputData(line);
    string input;
    string error;
    inputData >> input >> error;
    
    cout<<"=============================================================================================="<<endl;
    cout<<"Data: "<<input<<endl;
    cout<<"Error: "<<error<<endl;
    string inputError = introduceError(input, error);
    cout<<"CRC Section"<<endl;
    crcCheck(input, inputError);

    cout<<"Checksum Section"<<endl;
    checksumCheck(input, inputError);
    cout<<"2d Parity Section"<<endl;
    parityCheck(input, inputError);
    cout<<"=============================================================================================="<<endl;
        
    }
    
    return 0;
}