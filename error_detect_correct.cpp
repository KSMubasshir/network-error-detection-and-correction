#include<bits/stdc++.h>
#include<iostream>
#include<vector>
#include <windows.h>
using namespace std;

void setColor(string choice);

class TextAttr
{
    friend std::ostream& operator<<(std::ostream& out, TextAttr attr)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attr.value);
        return out;
    }
public:
    explicit TextAttr(WORD attributes): value(attributes) {}
private:
    WORD value;
};

#define FOREGROUND_WHITE ( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE )
#define FOREGROUND_CYAN  ( FOREGROUND_BLUE | FOREGROUND_GREEN )

int r;
int m=0;
float p=0;

vector<char> dataString;
vector<char> decodedData;
vector<int> dataStringAscii;
vector<int> dataStringWithHammingCode;
vector<int> columnWiseSerialisedData;
vector<int> receivedFrame;
vector<int> deserializedFrame;
vector<int> errorIndexes;
vector<int> errorIndexes2;
vector<int> generatorPolynomial;
vector<int> dataStringReceived;


void takeInput()
{
    char temp;
    cout<<"enter data string: ";
    while(1)
    {
        scanf("%c",&temp);
        if((int) temp==10) break;
        dataString.push_back(temp);
    }
    cout<<"enter num of data bytes in a row <m>: ";
    cin>>m;
    cout<<"enter probability <p>: ";
    cin>>p;
    cout<<"enter generator polynomal: ";
    char temp1;
    int j=0;
    while(1)
    {
        scanf("%c",&temp1);
        if(j==0)
        {
            j++;
            continue;
        }
        if((int) temp1==10) break;
        generatorPolynomial.push_back((int) temp1-48);
    }
}

void verifyInput()
{
    int dataSize=dataString.size();
    int rat;
    if(dataSize%m==0) return;
    rat=dataSize/m;
    int charTopad=0;
    charTopad=((rat+1)*m)-dataSize;
    for(int i=0; i<charTopad; i++)
        dataString.push_back('~');
}

void printVectorChar(vector<char> PrintVector)
{
    int length=PrintVector.size();
    for(int i=0; i<length; i++)
        cout<<PrintVector[i];
    cout<<endl;
}

void printVector(vector<int> PrintVector)
{
    int length=PrintVector.size();
    for(int i=0; i<length; i++)
        cout<<PrintVector[i];
    cout<<endl;
}

void printVectorErrorIndexColored(vector<int> PrintVector)
{
    int length=PrintVector.size();
    int errIndex=0;
    for(int i=0; i<length; i++)
    {
        if(p==0) setColor("white");
        else if(i==errorIndexes[errIndex])
        {
            errIndex++;
            setColor("red");
        }
        cout<<PrintVector[i];
        setColor("white");
    }
    cout<<endl;
}

void printVectorColorLastNBits(vector<int> PrintVector)
{
    int length=PrintVector.size();
    int colStart=length-generatorPolynomial.size()+1;
    for(int i=0; i<length; i++)
    {
        if(i==colStart) setColor("cyan");
        cout<<PrintVector[i];
    }
    setColor("white");
    cout<<endl;
}

void printVectorInt(vector<int> PrintVector)
{
    int length=PrintVector.size();
    for(int i=0; i<length; i++)
    {
        if(i%(m*8)==0) cout<<endl;
        cout<<PrintVector[i];
    }
    cout<<endl;
}

void printVectorHamming(vector<int> PrintVector)
{
    int d=0,idx=0;
    int length=PrintVector.size();
    for(int i=0; i<length; i++)
    {
        idx=i%((m*8)+r);
        setColor("white");
        if(idx==0)
        {
            d=0;
            cout<<endl;
        }
        if ((idx) == pow (2, d)-1)
        {
            setColor("green");
            d++;
        }
        cout<<PrintVector[i];
    }
    cout<<endl;
}

void printVectorDeserialized(vector<int> PrintVector)
{
    int d=0,idx=0;
    int length=PrintVector.size();
    int errIndex=0;
    //for(int i=0;i<errorIndexes2.size();i++) cout<<errorIndexes2[i]<<" ";
    for(int i=0; i<length; i++)
    {
        //cout<<"i="<<i<<"errindex="<<errorIndexes2[errIndex]<<endl;
        idx=i%(m*8+r);
        setColor("white");
        if(idx==0)
        {
            cout<<endl;
        }
        if(p==0) setColor("white");
        else if(i==errorIndexes2[errIndex]){
                //cout<<"Here"<<endl;
            setColor("red");
            errIndex++;
        }
        cout<<PrintVector[i];
    }
    cout<<endl;
}

void decToBinary()
{
    int num,bin,length,tempLength,diff;
    length=dataString.size();
    vector<int> temp;
    for(int i=0; i<length; i++)
    {
        num=(int) dataString[i];
        temp.clear();
        while (num > 0)
        {
            bin = num % 2;
            temp.push_back(bin);
            num /= 2;
        }
        reverse(temp.begin(),temp.end());
        tempLength=temp.size();
        if(tempLength!=8)
        {
            diff=8-tempLength;
            for(int i=0; i<diff; i++)
                dataStringAscii.push_back(0);
            for(int i=0; i<tempLength; i++)
                dataStringAscii.push_back(temp[i]);
        }
    }
}

void setColor(string choice)
{
    if(choice=="cyan")
    {
        cout<<TextAttr(FOREGROUND_INTENSITY | FOREGROUND_CYAN);
    }
    else if(choice=="red")
    {
        cout<<TextAttr(FOREGROUND_INTENSITY | FOREGROUND_RED);
    }
    else if(choice=="green")
    {
        cout<<TextAttr(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    }
    else
    {
        cout<<TextAttr(FOREGROUND_WHITE);
    }
    return;
}

bool hasOneInBinRepAtIndex(int num,int index)
{
    vector<int> temp;
    int bin=0;
    temp.clear();
    while (num > 0)
    {
        bin = num % 2;
        temp.push_back(bin);
        num /= 2;
    }
    reverse(temp.begin(),temp.end());

    if(temp[temp.size()-index-1]==1) return true;
    return false;
}

vector<int> hammingCodePerRow(int index)
{
    r=0;
    int d,d1;
    int a=m*8;
    while (a + r + 1 > pow (2, r))
        r++;
    int data[a + r];
    d = 0;
    d1 = 0;
    for (int i = 0; i < a + r; i++)
    {
        if ((i) == pow (2, d)-1)
        {
            data[i] = 0;
            d++;
        }
        else
        {
            data[i] = dataStringAscii[d1+index];
            d1++;
        }
    }

    d1 = 0;
    int min, max = 0, parity, s, j;
    /*Parity Bit Calculation */
    /*for(int i=0; i<a+r; i++)
        cout<<data[i];
    cout<<endl;*/
    for (int i = 0; i < a + r; i = pow (2, d1)-1)
    {
        d1++;
        parity = 0;
        j = i;
        s = i;
        min = 0;
        max = i;

        for (j; j < a + r;)
        {
            for (s = j;  s < a + r; min++, s++)
            {
                if (hasOneInBinRepAtIndex(s+1,d1-1) && data[s] == 1)
                {
                    parity++;
                }

            }
            j =s + i;
            min = 0;
        }

        if (parity % 2 == 0) // Even Parity
        {
            data[i] = 0;
        }
        else
        {
            data[i] = 1;
        }
    }
    vector<int> ret (data, data + sizeof(data) / sizeof(data[0]) );
    return ret;
}

void hammingCode()
{
    int numOfRows=dataString.size()/m;
    vector<int> temp;
    int tempSize=0;
    for(int i=0; i<numOfRows; i++)
    {
        temp=hammingCodePerRow(i*8*m);
        tempSize=temp.size();
        for(int j=0; j<tempSize; j++)
            dataStringWithHammingCode.push_back(temp[j]);
    }
}

void serialiseData()
{
    int rows=dataString.size()/m;
    int columns=m*8+r;
    for(int j=0; j<columns; j++)
    {
        for(int i=0; i<rows; i++)
        {
            columnWiseSerialisedData.push_back(dataStringWithHammingCode[j+(i*columns)]);
        }
    }
}

void addCRC()
{
    int degree=generatorPolynomial.size()-1;
    int dsize=degree+1;
    for(int i=0; i<degree; i++)
        columnWiseSerialisedData.push_back(0);
    int msize=columnWiseSerialisedData.size();
    int mess[msize],res[msize];
    int n = 0,m = 0,flag = 0,k = 0,count = 0;
    int rsize = msize;
    for(int i=0; i<msize; i++)
        res[i] = columnWiseSerialisedData[i];
    for(int i=0; i<dsize; i++)
    {
        res[i] = columnWiseSerialisedData[i]^generatorPolynomial[i];
        n = i;
    }
    while(n <= msize)
    {
        while(flag == 0)
        {
            if(res[m] == 0)
                m++;
            else
                flag = 1;
        }
        flag = 0;
        for(int i=m; i<(dsize+m); i++)
        {
            if(res[i] == 0 || res[i] == 1)
                count++;
        }
        if(count == dsize)
        {
            for(int i=m; i<(dsize+m); i++)
            {
                res[i] = res[i]^generatorPolynomial[k];
                k++;
            }
            k = 0;
        }
        else
            break;
        count = 0;
        n = (dsize+m)+1;
    }
    rsize = msize-1;
    for(int i=1; i<dsize; i++)
    {
        columnWiseSerialisedData[rsize] = res[rsize];
        rsize--;
    }
}

bool introduceError()
{
    if(p==0) return false;
    bool TrueFalse = ((rand() %100) <= (p*100));
    return TrueFalse;
}

void sendWithErrProbP()
{
    int length=columnWiseSerialisedData.size();
    for(int i=0; i<length; i++)
    {
        if(introduceError())
        {
            errorIndexes.push_back(i);
            receivedFrame.push_back(columnWiseSerialisedData[i]^1);
        }
        else
            receivedFrame.push_back(columnWiseSerialisedData[i]);
    }
}

void checkCRC()
{
    int n = 0,flag = 0,m = 0,count = 0,k = 0,flag1 = 0;
    int rmsize = receivedFrame.size();
    int rrsize = rmsize;
    int dsize=generatorPolynomial.size();
    int rres[rrsize],rrem[rrsize];

    for(int i=0; i<rmsize; i++)
        rres[i] = receivedFrame[i];
    for(int i=0; i<dsize; i++)
    {
        rres[i] = receivedFrame[i]^generatorPolynomial[i];
        n = i;
    }
    while(n <= rmsize)
    {
        while(flag == 0)
        {
            if(rres[m] == 0)
                m++;
            else
                break;
        }
        flag = 0;
        for(int i=m; i<(dsize+m); i++)
        {
            if(rres[i] == 0 || rres[i] == 1)
                count++;
        }
        if(count == dsize)
        {
            for(int i=m; i<(dsize+m); i++)
            {
                rres[i] = rres[i]^generatorPolynomial[k];
                k++;
            }
            k = 0;
        }
        else
            break;
        count = 0;
        n = (dsize+m) + 1;
    }
    rrsize = rmsize-1;
    for(int i=dsize-1; i>0; i--)
    {
        rrem[i] = rres[rrsize];
        rrsize--;
    }
    for(int i=1; i<dsize; i++)
    {
        if(rrem[i] == 1)
        {
            flag1 = 1;
            break;
        }
    }
    cout<<"\nresult of CRC checksum matching: ";
    if(flag1 == 0)
        cout<<"no error detected\n";
    else
        cout<<"error detected\n";
}

void removeCRCandDeserialize()
{
    int rows=dataString.size()/m;
    int columns=m*8+r;
    deserializedFrame.clear();
    int errIndex=0;

    for(int i=0; i<rows; i++)
    {
        for(int j=0; j<columns; j++)
        {
            deserializedFrame.push_back(receivedFrame[(j*rows)+i]);
        }
    }
    int errSize=errorIndexes.size();
    int translatedIndex=-1;
    for(int i=0;i<errSize;i++){
        translatedIndex=errorIndexes[i]/rows+(errorIndexes[i]%rows)*columns;
        if(errorIndexes[i]>=rows*columns) {
                continue;
        }
        errorIndexes2.push_back(translatedIndex);
    }
    sort(errorIndexes2.begin(),errorIndexes2.end());
}

void errorCorrectionPerRow(int index){
    int errIndex=0;
    int d1=0;
    int a=m*8;
    int data[a + r];
    int min, max = 0, parity, s, j;
    vector<int> parityVector;

    for (int i = 0; i < a + r; i ++){
        data[i]=deserializedFrame[index+i];
    }

    for (int i = 0; i < a + r; i = pow (2, d1)-1)
    {
        d1++;
        parity = 0;
        j = i;
        s = i;
        min = 0;
        max = i;
        for (j; j < a + r;)
        {
            for (s = j;  s < a + r; min++, s++)
            {
                if (hasOneInBinRepAtIndex(s+1,d1-1) && data[s] == 1){
                    parity++;
                }

            }
            j =s + i;
            min = 0;
        }
        if (parity % 2 == 0) // Even Parity
        {
            parityVector.push_back(0);
        }
        else
        {
            parityVector.push_back(1);
        }
    }
    reverse(parityVector.begin(),parityVector.end());

    int parityLength=parityVector.size();
    for(int i=0;i<parityLength;i++){
        errIndex+=pow(2,parityLength-1-i)*parityVector[i];
    }
    if( errIndex!=0 && errIndex<a+r )
        deserializedFrame[index+errIndex-1]^=1;
}

void errorCorrection(){
    int numOfRows=dataString.size()/m;
    for(int i=0; i<numOfRows; i++)
    {
        errorCorrectionPerRow(i*(8*m+r));
    }
}

void removeCheckBits(){
    int d=0,idx=0;
    int length=deserializedFrame.size();
    dataStringReceived.clear();
    for (int i = 0; i < length ; i++)
    {
        idx=i%(m*8+r);
        if(idx==0) d=0;
        if ((idx) == pow (2, d)-1)
        {
            d++;
        }
        else
        {
            dataStringReceived.push_back(deserializedFrame[i]);
        }
    }
}

char asciiToChar(int index){
    int ascii=0;
    for(int i=0;i<8;i++){
        ascii+=pow(2,7-i)*dataStringReceived[index+i];
    }
    char ret=ascii;
    return ret;
}

void decode(){
    int length=dataStringReceived.size();
    for(int i=0;i<length;i+=8){
        decodedData.push_back(asciiToChar(i));
    }
}

int main()
{
    takeInput();
    verifyInput();
    cout<<"\ndata string after padding: ";
    printVectorChar(dataString);
    decToBinary();
    cout<<"\ndata block <ascii code of m characters per row>:";
    printVectorInt(dataStringAscii);
    hammingCode();
    cout<<"\ndata block after adding check bits:";
    printVectorHamming(dataStringWithHammingCode);
    serialiseData();
    cout<<"\ndata bits after column-wise serialization:\n";
    printVector(columnWiseSerialisedData);
    addCRC();
    cout<<"\ndata bits after appending crc checksum <sent frame>:\n";
    printVectorColorLastNBits(columnWiseSerialisedData);
    sendWithErrProbP();
    cout<<"\nreceived frame:\n";
    printVectorErrorIndexColored(receivedFrame);
    checkCRC();
    removeCRCandDeserialize();
    cout<<"\ndata block after removing CRC checksum bits:";
    printVectorDeserialized(deserializedFrame);
    errorCorrection();
    removeCheckBits();
    cout<<"\ndata block after removing check bits:";
    printVectorInt(dataStringReceived);
    decode();
    cout<<"\noutput frame: ";
    printVectorChar(decodedData);
    return 0;
}
