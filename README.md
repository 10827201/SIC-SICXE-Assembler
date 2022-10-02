# -SIC-SICXE-Assembler
開發平台 :Windows  
使用開發環境 : Dev C++  
使用的程式語言 : C++   
所選擇的組合語言(x86 , SIC ) : SIC / SICXE  
功能 :  
將input檔，一行行進行切token，並且判斷此行是否有syntax error，若有則不進行翻譯成object code，若無則進行翻譯，若此行有宣告時，將宣告存入symtab。重複動作是檔案讀取完畢。此為pass 1。
再來進行pass2 將forward reference處理完畢。

流程:  
一、	一開始輸入檔名，判斷檔案使否存在，若不存在則重新輸入。  
二、	以一行行的方式讀取資料內容。  
三、	將內容以white space切割成各自單一字串，但若遇到單引號的話切到下個單引號。  
四、	判斷字串使否位於已存在的table內，若位於table內，將位於的table以及位置連同字串存入vector。  
五、	若沒找到則將字串以table4內的符號做切割，並且將切下來的小token進行分類。其中類別為字串、數字、symbol、以及原本存在table內的。  
六、	若分類於字串、數字以及symbol，則在各自的table進行hash，將位於的table以及位置連同字串存入vector。  
七、	若位於原本的table，則與步驟三相同。  
八、	重複步驟三到六直到一行結束。  
九、	對此行進行syntax error檢查，若有錯誤不進行翻譯。  
十、	將此行的內容翻譯成object code。  
十一、	重複步驟二到十直到檔案結束。  
十二、	再將存放檔案內容以及objcode的vector重頭再進行一次翻譯，來處理fordward reference的問題。  
十三、	將vector內的資料寫入output檔中。  
十四、	Symbol、數字、字串的table分別寫成table5、6、7輸出  

*SIC與SICXE流程相同，僅有中間對於指令做翻譯的方法不同，因格式不同故方法不相同。  
