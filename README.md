

======== AUTO WATER MOTAR CONTROL V-2 =======

Function Modes

1. Auto Mode (No save voltage)
  - Motar Run when water level low than 100%  
  - Motar Stop when water level is 100%

2. Manual Mode
  - Motar relay always open, you need to control by on/off swithches of Motar
  - Not related with sensor 
  - This Mode is prefer for external usages that not related water level sensor

3. Auto Save Mode (Save voltage)
  - Motar Run when water level low and voltage is higher than save voltage
  - Motar Stop when water level is 100% or current voltage is lower than save voltage
  - This Mode is perfer for Motar's saving by using low voltage protection

  - *Low Voltage Protection*
    - When current ACV low than SV, Motar will Stop.
    - If this situation happens more than 2 time quickly within 1 min, system will sure that is low voltage Problem.
    - If low votage problem found, system will wait perfect safe voltage to restart the motar. 
    - Motar will restart when current ACV is equal or higher than safe voltage.


Protections and Adjustments

- If no water flow after 1 min of motar start in auto save and no save modes, motar will stop immediately and lcd will show "No Water Flowing"

- You can adjust the save voltage(SV) in auto save mode by pressing the adjust btn for 3s.
  Adjust the SV by Potentionmeter and then press the adjust button for 3s to release.


# ======= AUTO WATER MOTAR CONTROL V-2 =======

### **အသုံးပြုနိုင်သော စနစ်များ (Function Modes)**

#### **၁။ အလိုအလျောက်စနစ် - ဗို့အားမသတ်မှတ်ထား (Auto Mode - No save voltage)**
* **မော်တာလည်ပတ်ခြင်း:** ရေပမာဏ ၁၀၀% အောက် လျော့နည်းသွားပါက မော်တာ အလိုအလျောက် လည်ပတ်မည်။
* **မော်တာရပ်တန့်ခြင်း:** ရေပမာဏ ၁၀၀% ပြည့်သွားပါက မော်တာ အလိုအလျောက် ရပ်တန့်မည်။

#### **၂။ ကိုယ်တိုင်ထိန်းချုပ်စနစ် (Manual Mode)**
* **လုပ်ဆောင်ချက်:** မော်တာ Relay ကို အမြဲတမ်း ဖွင့်ထားပေးမည်ဖြစ်ပြီး မော်တာ၏ အဖွင့်/အပိတ် ခလုတ်များဖြင့်သာ ကိုယ်တိုင် ထိန်းချုပ်ရမည်။
* **အာရုံခံကိရိယာ:** ရေအနိမ့်အမြင့် အာရုံခံကိရိယာ (Sensor) နှင့် သက်ဆိုင်မှု မရှိပါ။
* **အသုံးပြုမှု:** ဤစနစ်ကို အာရုံခံကိရိယာနှင့် မပတ်သက်သော ပြင်ပကိစ္စရပ်များတွင် အသုံးပြုရန် သင့်တော်သည်။

#### **၃။ အလိုအလျောက် လုံခြုံရေးစနစ် - ဗို့အားသတ်မှတ်ထား (Auto Save Mode)**
* **မော်တာလည်ပတ်ခြင်း:** ရေပမာဏ လျော့နည်းနေပြီး လက်ရှိ ဗို့အားသည် သတ်မှတ်ထားသော လုံခြုံစိတ်ချရသည့် ဗို့အား (Save Voltage) ထက် မြင့်မှသာ လည်ပတ်မည်။
* **မော်တာရပ်တန့်ခြင်း:** ရေပမာဏ ၁၀၀% ပြည့်သွားလျှင်သော်လည်းကောင်း၊ လက်ရှိဗို့အားသည် သတ်မှတ်ထားသော ဗို့အားအောက် လျော့နည်းသွားလျှင်သော်လည်းကောင်း ရပ်တန့်ပေးမည်။
* **အသုံးပြုမှု:** ဤစနစ်သည် ဗို့အားနည်းခြင်းကြောင့် မော်တာပျက်စီးခြင်းမှ ကာကွယ်ရန်အတွက် အသင့်တော်ဆုံး ဖြစ်သည်။

---

### **ဗို့အားနည်းခြင်းမှ ကာကွယ်သည့်စနစ် (Low Voltage Protection)**
* လက်ရှိ AC ဗို့အားသည် သတ်မှတ်ထားသော ဗို့အား (SV) ထက် နည်းသွားပါက မော်တာ ရပ်တန့်သွားမည်။
* ဤအခြေအနေသည် ၁ မိနစ်အတွင်း ၂ ကြိမ်ထက်ပို၍ ဖြစ်ပေါ်နေပါက စနစ်မှ **"Low Voltage Problem"** အဖြစ် သတ်မှတ်မည်။
* ဗို့အားနည်းသော ပြဿနာ ဖြစ်ပေါ်ပါက မော်တာ ပြန်လည်မလည်ပတ်မီ လုံခြုံစိတ်ချရသော ဗို့အားအဆင့်သို့ ရောက်ရှိရန် စနစ်မှ စောင့်ဆိုင်းနေမည်။
* လက်ရှိ AC ဗို့အားသည် သတ်မှတ်ထားသော ဗို့အားနှင့် ညီမျှခြင်း သို့မဟုတ် ပိုများလာမှသာ မော်တာ ပြန်လည်လည်ပတ်မည်။

---

### **ကာကွယ်မှုများနှင့် ချိန်ညှိခြင်း (Protections and Adjustments)**

> [!CAUTION]
> **ရေမထွက်ပါက အလိုအလျောက်ရပ်တန့်ခြင်း:** Auto သို့မဟုတ် Auto Save Mode များတွင် မော်တာစတင်လည်ပတ်ပြီး ၁ မိနစ်ကြာသည်အထိ ရေထွက်မလာပါက မော်တာ ချက်ချင်းရပ်တန့်သွားမည်ဖြစ်ပြီး LCD မျက်နှာပြင်တွင် **"No Water Flowing"** ဟု ပြသမည်။

#### **Save Voltage (SV) ကို ချိန်ညှိနည်း**
1. **Adjust Button** ကို ၃ စက္ကန့်ကြာ ဖိထားပါ။
2. **Potentiometer** ကို အသုံးပြု၍ လိုချင်သော ဗို့အားပမာဏကို ချိန်ညှိပါ။
3. ပြန်လည်သိမ်းဆည်းရန် **Adjust Button** ကို ၃ စက္ကန့်ကြာ ထပ်မံဖိထားပေးပါ။
