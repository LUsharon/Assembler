#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *inputFile, *outputFile, *addressFile, *symbolFile, *objectFile ,*onlyaddressFile;

char op_key[60][10] = { "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF", "DIVR",
        "FIX", "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL", "LDS",
        "LDT", "LDX", "LPS", "MUL", "MULF", "MULR", "NORM", "OR", "RD", "RMO", "RSUB", "SHIFTL", "SHIFTR",
        "SIO", "SSK", "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX", "SUB", "SUBF",
        "SUBR", "SVC", "TD", "TIO", "TIX", "TIXR", "WD" };
// char op_val[60][10] = { "18", "58", "90", "40", "B4", "28", "88", "A0", "24", "64", "9C", "C4", "C0", "F4", "3C",
//         "30", "34", "38", "48", "00", "68", "50", "70", "08", "6C", "74", "04", "E0", "20", "60", "98", "C8",
//         "44", "D8", "AC", "4C", "A4", "A8", "F0", "EC", "0C", "78", "54", "80", "D4", "14", "7C", "E8", "84",
//         "10", "1C", "5C", "94", "B0", "E0", "F8", "2C", "B8", "DC" };
int op_val[60] = { 0x18, 0x58, 0x90, 0x40, 0xB4, 0x28, 0x88, 0xA0, 0x24, 0x64, 0x9C, 0xC4, 0xC0, 0xF4, 0x3C,
        0x30, 0x34, 0x38, 0x48, 0x00, 0x68, 0x50, 0x70, 0x08, 0x6C, 0x74, 0x04, 0xE0, 0x20, 0x60, 0x98, 0xC8,
        0x44, 0xD8, 0xAC, 0x4C, 0xA4, 0xA8, 0xF0, 0xEC, 0x0C, 0x78, 0x54, 0x80, 0xD4, 0x14, 0x7C, 0xE8, 0x84,
        0x10, 0x1C, 0x5C, 0x94, 0xB0, 0xE0, 0xF8, 0x2C, 0xB8, 0xDC };

//判斷哪些加2 加3那些加4(看+號) 
int op_addr[60]={3,3,2,3,2,3,3,2,3,3,2,
        1,1,1,3,3,3,3,3,3,3,3,3,3,3,
        3,3,3,3,3,2,1,3,3,2,3,2,2,
        1,3,3,3,3,3,3,3,3,3,3,3,3,3,
        2,2,3,1,3,2,3
};
int optable_size = sizeof(op_key) / sizeof(op_key[0]);

int two_three[500]={};

int optable(char check[10]){
    for (int i = 0; i < optable_size; i++) {
        if (strcmp(check, op_key[i]) == 0) {//檢查是不是opcode
            return i;//回傳第幾個
        }
    }
    return 1000;
}
struct symbol{
    int address;
    char symbol[10];
};
struct symbol symboltable[100];
int symboltable_index = 0;
int findlabel(char find[10]){
    for (int i = 0; i < symboltable_index; i++) {
        //printf("~%d:%s %x\n",i,symboltable[i].symbol,symboltable[i].address);
        if (strcmp(find, symboltable[i].symbol) == 0) {//檢查
            return symboltable[i].address;
        }
    }
    return 0xfff;
}
char XATS[8] = {'X','A','T','S','L','B','F'};
int XATS_val[8] = {1,0,5,4,2,3,6};

int XATStable(char find[2]){

    for (int i = 0; i < 8; i++) {
        //printf("~%d:%s %x\n",i,symboltable[i].symbol,symboltable[i].address);
        if (find[0] == XATS[i]) {//檢查
            return XATS_val[i];
        }
    }
}  

int add_length = 0;
char name[10] = "0";
char start[10] = "0";
char start_add[7] = "000000";//預設
char start_modify[6] = "0";//開始位址
int start_int = 0x56;   
int address = 0x0;
int endprogram_address = 0x0;
int modify_record[5] = {0x0};
int text_record[100] = {0x0};
int modify_record_index = 0;
int text_record_index = 0;
void ObjectFile() {//generate object program

    addressFile = fopen("address.txt", "r");
    fprintf(outputFile, "H^%s  ^%06s^%06X\n", name,start_add,endprogram_address);
    fprintf(outputFile,"T^%06X^05\n", text_record[0]);
    // for(int i=0;i<9;i++){
    //     fprintf(outputFile,"T^%06X^05\n", text_record[i]);
    // }
    printf("%x",modify_record[0]);
    for (int i = 0; i < modify_record_index; i++){
        fprintf(outputFile,"M^%06X^05\n", modify_record[i]);
    }  
    fprintf(outputFile, "E^%06s",start_add);
}

int main() {

    // 打开输入文件
    inputFile = fopen("input.txt", "r");
    if (inputFile == NULL) {
        printf("無法打開輸入文件.\n");
        return 1;
    }

    // 创建并打开输出文件
    outputFile = fopen("object_program.txt", "w");
    if (outputFile == NULL) {
        printf("無法輸出文件.\n");
        return 1;
    }
    objectFile = fopen("object_code.txt", "w");
    if (objectFile == NULL) {
        printf("無法輸出文件.\n");
        return 1;
    }
    addressFile = fopen("address.txt", "w");
    if (addressFile == NULL) {
        printf("無法輸出文件.\n");
        return 1;
    }
    symbolFile = fopen("symboltable.txt", "w");
    if (addressFile == NULL) {
        printf("無法輸出文件.\n");
        return 1;
    }

    
    

    

    fscanf(inputFile, "%s %s %s", name, start, start_modify);//輸入COPY START 5

    add_length = strlen(start_modify);
    
    // 根据add_length修改start_add的对应位数
    strncpy(start_add + (6 - add_length), start_modify, add_length);
    address = atoi(start_modify);
    fprintf(addressFile, "%s %s %s %s\n",start_modify, name, start, start_modify);//輸出第一行address
    

    //label instruction operand
    char one[10]="0";
    char two[10]="0";
    char three[10]="0";
    char label[10]="0";
    char instruction[10]="0";
    char operand[10]="0";

    int alladdress[500] = {0x0};
    int address_index = 0;
    
    int two_three_index = 0;
    while(fscanf(inputFile, "%s", one)!=EOF){
        int find = optable(one);
        
        if(strcmp("END",one)==0){
            fscanf(inputFile, "%s", two);
            alladdress[address_index] = address;
            address_index++;
            fprintf(addressFile, "%x %s %s\n",address, one, two);
            two_three[two_three_index]=2;
            two_three_index+=1;
            endprogram_address = address;
            
        }
        else if(strcmp("BASE",one)==0){//沒有address
            fscanf(inputFile, "%s", two);
            fprintf(addressFile, " %s %s\n", one, two);
            two_three[two_three_index]=2;
            two_three_index+=1;
        }
        else if(strcmp("RSUB",one)==0){//有address
            alladdress[address_index] = address;
            address_index++;
            fprintf(addressFile, "%x %s\n",address, one);
            address+=3;
            two_three[two_three_index]=1;
            two_three_index+=1;
        }
        else if(one[0]=='+'){//是opcode format4 代表是2個的 instruction operand
            fscanf(inputFile, "%s", two);
            alladdress[address_index] = address;
            address_index++;
            
            strcpy(instruction,one);
            strcpy(operand,two);
            fprintf(addressFile, "%x %s %s\n", address, instruction, operand);
            address += 4;
            two_three[two_three_index]=2;
            two_three_index+=1;
        }
        else if(find <= 60){//是opcode 代表是2個的 instruction operand
            fscanf(inputFile, "%s", two);
            strcpy(instruction,one);
            strcpy(operand,two);
            //printf("opcode:%s",instruction);
            alladdress[address_index] = address;
            address_index++;
           
            fprintf(addressFile, "%x %s %s\n", address, instruction, operand);
            address += op_addr[find];
            two_three[two_three_index]=2;
            two_three_index+=1;
        }
        else{//不是opcode 代表是3個的 label instruction operand
            fscanf(inputFile, "%s", two);
            strcpy(label,one);
            strcpy(instruction,two);
            fscanf(inputFile, "%s",operand);
            strcpy(symboltable[symboltable_index].symbol ,one);
            symboltable[symboltable_index].address = address;
            symboltable_index++;
            alladdress[address_index] = address;
            address_index++;
            fprintf(symbolFile, "%s %x\n", label, address);//symbol table
            fprintf(addressFile, "%x %s %s %s\n", address, label, instruction, operand);
            two_three[two_three_index]=3;
            two_three_index+=1;

            if(instruction[0]=='+'){
                address += 4;
            }
            else if(strcmp(instruction,"BYTE")==0){
                int temp = strlen(operand);
                if(operand[0]=='C'){
                    address+= (temp-3);
                }
                else if(operand[0]=='X'){
                    address+= (temp-3)/2;
                }
                
            }
            else if(strcmp(instruction,"RESW")==0){
                address += 3*atoi(operand);
      
            }
            else if(strcmp(instruction,"RESB")==0){
                address += atoi(operand);
  
            }
            else{
                find = optable(instruction);
                if(find <= 60){
                    address += op_addr[find];
                    //printf("opcode:%s",instruction);
                }
                else{
                    printf("##not found\n");
                }
            }
        }
       
    }
    int end_address = address;
    address_index = 1;
    printf("done pass1 ,address.txt symboltable.txt.\n");
//######################################################################
    fclose(inputFile);
    fclose(addressFile);
    fclose(symbolFile);
    //pass2
    addressFile = fopen("address.txt", "r");
    address=0;
    label[0]= '0';
    fscanf(addressFile, "%x %s %s %s", &address,label,instruction,operand);
    fprintf(objectFile,"%x %s %s %s\n", address,label,instruction,operand);
    int lines_sum = two_three_index;
    two_three_index = 0;
    
    int ni = 3;
    int x = 0;
    int bp = 0;
    int e = 0;
    int xbpe = 2;

    while(fscanf(addressFile, "%x", &address)!=EOF){
        
        fscanf(addressFile, "%s", instruction);//預設是2個的
        if(strcmp("+JSUB",instruction) == 0){
            modify_record[modify_record_index] = address+1;
            modify_record_index++;
        }
        char object_code[9] = "0";
        if(strcmp("SE",instruction)==0){
            
            fscanf(addressFile, "%s", operand);            
            fprintf(objectFile,"    BASE %s\n", operand);
            two_three_index++;
        }
        else{
            int find = 0;
            //fprintf(addressFile, "%x %s %s %s\n", address, label, instruction, operand);
            if(two_three[two_three_index]<=2){// address, instruction, operand 2個的
                fscanf(addressFile, "%s", operand);
                int pc = 0x0;
                int ta = 0x30;
                int disp = 0x0;
                pc = alladdress[address_index];//下個位置
                address_index++;
                if(strcmp(instruction,"RSUB")==0){
                    find = optable(instruction);
                    fprintf(objectFile, "    %s 4F0000\n", instruction);
                }
                else if(strcmp(instruction,"END")==0){
                    fprintf(objectFile, "    %s %s\n", instruction, operand);
                }
                else if(op_addr[optable(instruction)] == 2){
                    
                    find = optable(instruction);
                    if(strcmp("COMPR",instruction) == 0){
                        char temp[10] = "0";//裝XATS
                        strncpy(temp, operand, 1);
                        int x = XATStable(temp);
                        strncpy(temp, operand+2, 3);
                        int y = XATStable(temp);
                        fprintf(objectFile, "%x %s %s %02X%01X%01X\n", address, instruction, operand ,op_val[find],x,y);
                        char result[9];  // 为结果分配足够的空间，包括结尾的空字符
                        
                        sprintf(result, "%02X%01X%01X", op_val[find], x, y);
                        sscanf(result, "%X", &text_record[text_record_index]);
                        text_record_index++;
                    }
                    else{
                        
                        int x = XATStable(operand);
                        fprintf(objectFile, "%x %s %s %02X%01X0\n", address, instruction, operand ,op_val[find],x);
                    }
                    
                }
                else{
                    //printf("operand:%s\n",operand);
                    
                    if(operand[0] == '#'){
                        
                        ni = 1;
                        xbpe = 2;
                        char temp[10] = "0";//裝去#operand
                        strncpy(temp, operand+1, 10);
                        ta = findlabel(temp);  //address
                        find = optable(instruction);//
                        if(instruction[0] == '+'){
                            xbpe = 1;
                            char cut[10] = "0";
                            strncpy(cut, instruction+1, 10);
                            find = optable(cut);
                            int hex = atoi(temp);
                            //printf("$%s hex%x\n",temp,hex);
                            fprintf(objectFile, "%x %s %s %02X%01X%05X\n", address, instruction, operand, op_val[find]+ni,xbpe,hex);
                        }
                        else{
                            if(ta != 0xfff){//在表上 

                                disp = ta - pc;
                                fprintf(objectFile, "%x %s %s %02X%01X%03X\n", address, instruction, operand, op_val[find]+ni,xbpe,disp);
                            }
                            else if('0'<=temp[0]<='9'){//為#0 #4096這種
                                int hex = atoi(temp);
                                fprintf(objectFile, "%x %s %s %02X%04X\n", address, instruction, operand, op_val[find]+ni,hex);
                            }
                        }
                        
                    
                    }
                    else if(instruction[0] == '+'){ //format 4 
                        char temp[10] = "0";
                        strncpy(temp, instruction+1, 10);
                        find = optable(temp);
                        ta = findlabel(operand);
                        ni = 3;
                        xbpe = 1;
                        fprintf(objectFile, "%x %s %s %02X%01X%05X\n", address, instruction, operand, op_val[find]+ni,xbpe,ta);
                    }
                    else if(operand[0] == '@'){
                        ni = 2;
                        xbpe = 2;
                        char temp[10] = "0";
                        strncpy(temp, operand+1, 10);
                        ta = findlabel(temp);  
                        find = optable(instruction);
                        disp = ta - pc;
                        fprintf(objectFile, "%x %s %s %02X%01X%03X\n", address, instruction, operand, op_val[find]+ni,xbpe,disp);
                         
                    }
                    else if (strcmp(instruction,"STCH") == 0||strcmp(instruction,"LDCH")==0 ){
                        ni=3;
                        xbpe =12;
                        find = optable(instruction);
                        int x = XATStable("B");
                        fprintf(objectFile, "%x %s %s %02X%01X%03X\n", address, instruction, operand, op_val[find]+ni,xbpe,x);
                    }
                    else{//普通的
                    
                        ni=3;
                        xbpe =2;
                        find = optable(instruction);
                        ta = findlabel(operand);
                        disp = ta - pc;
                        if(disp < -2056){
                            xbpe = 4;
                            fprintf(objectFile, "%x %s %s %02X%01X000\n", address,instruction, operand, op_val[find]+ni,xbpe);
                        }
                        else{
                            disp &= 0x00000FFF;
                            fprintf(objectFile, "%x %s %s %02X%01x%03X\n", address, instruction, operand, op_val[find]+ni,xbpe,disp);
                        }
                        
                    }
                }               
            }
            else{//3個的
                strcpy(label,instruction);
                fscanf(addressFile, "%s %s",instruction,operand);
                if(strcmp("+JSUB",instruction) == 0){
                    modify_record[modify_record_index] = address+1;
                    modify_record_index++;
                }
                find = optable(instruction);
                int ta = findlabel(operand);
                int disp = 0x0;
                int pc = 0x0;
                pc = alladdress[address_index];//下個位置
                address_index++;
                if(instruction[0] == '+'){
                    //再擷取字
                    char temp[10] = "0";
                    strncpy(temp, instruction+1, 10);
                    find = optable(temp);
                    ta = findlabel(operand);
                    ni = 3;
                    xbpe = 1;
                    fprintf(objectFile, "%x %s %s %s %02X%01X%05X\n", address, label,instruction, operand, op_val[find]+ni,xbpe,ta);
                }
                else if(strcmp(instruction,"BYTE") == 0){
                    if(operand[0] == 'C'){
                        char temp[10] = "0";
                        
                        strncpy(temp, operand+2, 3);
                        fprintf(objectFile, "%x %s %s %s %02X%02X%02X\n", address, label,instruction, operand,temp[0],temp[1],temp[2]);
                    }
                    else if(operand[0] == 'X'){
                        char temp[10] = "0";
                        strncpy(temp, operand+2, 2);
                        fprintf(objectFile, "%x %s %s %s %02s\n", address, label,instruction, operand,temp);
                    }
                }
                else if((strcmp(instruction,"RESW") == 0 ) || (strcmp(instruction,"RESB") == 0 )){
                    fprintf(objectFile, "%x %s %s %s\n", address, label,instruction, operand);
                }
                else if(op_addr[optable(instruction)] == 2){
                    find = optable(instruction);
                    int x = XATStable(operand);
                    fprintf(objectFile, "%x %s %s %s %02X%01X0\n", address, label,instruction, operand ,op_val[find],x);
                }
                else{
                    disp = ta - pc;
                    if(disp < -2056){
                        xbpe = 4;
                        fprintf(objectFile, "%x %s %s %s %02X%01X000\n", address, label,instruction, operand, op_val[find]+ni,xbpe);
                    }
                    else{
                        xbpe = 2;
                        ni = 3;
                        fprintf(objectFile, "%x %s %s %s %02X%01X%03X\n", address, label,instruction, operand, op_val[find]+ni,xbpe,disp);
                    }
                }             
            }
            two_three_index++;           
        }
    }
    fclose(addressFile);
    ObjectFile();
    fclose(objectFile);
    fclose(outputFile);
    return 0;
}
