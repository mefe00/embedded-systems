//  <<<------------------------------------------------------------------------------->>>
//  <<<------------------------------Driver Hakkında---------------------------------->>>
//  <<<------------------------------------------------------------------------------->>>

//  <<<-----------------------------Tanıtım ve Bilgilendirme-------------------------->>>
// Bu sürücü, hoverboard motorlarda olan BLDC motorlar için Hall sensörlerinden elektriksel açı bilgisi okumak için tasarlanmıştır.
// Sürücüde 3 tane Hall sensöründen gelen dijital sinyaller okunur ve bu sinyaller ile elektriksel açı değeri hesaplanır.
// Eğer daha hassas açı bilgisi isteniyorsa bu Hall sensörleri yerine manyetik encoder gibi daha yüksek çözünürlüklü (14-bit) encoderlar kullanılmalıdır.
// Motoru eğer FOC kontrol yöntemi ile sürmeyi planlıyorsanız ve robot kol gibi sistemlerde kullanacaksanız manyetik encoder kullanmadan açı bilgisini FOC algoritmasına girmek baş ağrıtabilir.
//  <<<------------------------------------------------------------------------------->>>

//  <<<-------------------------------------Yöntem------------------------------------>>>
// Üç Hall sensöründen gelen dijital sinyaller 3 bitlik bir sektör bilgisi oluşturur.
// Bu sektör bilgisi 1-6 arasında değişir ve her sektör 60 derecelik bir elektriksel açı aralığını temsil eder.
// Sektör bilgisi kullanılarak temel elektriksel açı değeri belirlenir.
// Ayrıca her sektör geçişinde geçen süre ölçülerek sektör içi açı doğrusal olarak hesaplanır.
// Toplam elektriksel açı, sektör taban açısı ile sektör içi açının toplamı olarak elde edilir.
// | Ha | Hb | Hc | Sektör | Elektriksel Açı Aralığı | Açı ortası FOC için |
// |----|----|----|--------|-------------------------|---------------------|
// | 0  | 0  | 1  |   1    |      330 - 30           |        0            |
// | 0  | 1  | 0  |   2    |      90 - 150           |        120          |
// | 0  | 1  | 1  |   3    |      30 - 90            |        60           |
// | 1  | 0  | 0  |   4    |      210 - 270          |        240          |
// | 1  | 0  | 1  |   5    |      270 - 330          |        300          |
// | 1  | 1  | 0  |   6    |      150 - 210          |        180          |  

// NOT: Tipik bir 3 fazlı BLDC motorun normal çalışmasında tüm hall sensörlerinin aynı anda 0 veya 1 verdiği görülmez.
//  <<<------------------------------------------------------------------------------->>>

//  <<<---------------------------------Kullanımı------------------------------------->>>

// 1. HALL_Init fonksiyonu ile Hall sensörleri için gereken başlangıç ayarlarını yapın.
//    Bu fonksiyona Hall sensörleri için kullanılan Timer'ın adresini girin.
// 2. Hall sensörlerine bağlı GPIO pinlerinde harici kesme (EXTI) yapılandırması yapın.
// 3. HAL_GPIO_EXTI_Callback fonksiyonu içinde HALL_Process_Interrupt fonksiyonunu çağırın ve tetiklenen GPIO pinini parametre olarak geçirin.

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  if (GPIO_Pin == GPIO_PIN_0 || GPIO_Pin == GPIO_PIN_1 || GPIO_Pin == GPIO_PIN_2) // Sadece Hall pinlerini kontrol et
//  {
//    HALL_Process_Interrupt(GPIO_Pin);
//  }
  // Diğer EXTI kesmeleri buraya
//}

// 4. HALL_GetElectricalAngle fonksiyonu ile mevcut elektriksel açıyı derece cinsinden okuyabilirsiniz.
//    float angle = HALL_GetElectricalAngle();
// 5. HALL_GetCurrentSector fonksiyonu ile mevcut sektörü (1-6) okuyabilirsiniz.


//  <<<------------------------------------------------------------------------------->>>


//  <<<------------------------------------------------------------------------------->>>
//  <<<------------------------------------------------------------------------------->>>
//  <<<------------------------------------------------------------------------------->>>

#include "Hall.h"

//  <<<------------------------------------------------------------------------------->>>
//  <<<------ Özel Değişkenler ------>>>
//  <<<------------------------------------------------------------------------------->>>


static TIM_HandleTypeDef *HALL_htim; // Hall sensörleri için kullanılan Timer'ın adresi
volatile uint8_t sector = 0; // En son okunan 60 derecelik sektör (1-6)
volatile uint32_t sector_previous_time = 0; // Önceki sektör geçiş süresi (Sektörün içinden detaylı açı hesabı için)

const uint8_t HALL_SECTOR_MAP[8]={
         //  Açı,  Hall_a, Hall_b, Hall_c, Sektör
    0,   // 000 (Geçersiz---> Tipik 3 fazlı bir BLDC motorun normal çalışmasında tüm hall sensörlerinin aynı anda 0 verdiği görülmez),
    330, // 001 (Sektör 1)
    90,  // 010 (Sektör 2)
    30,  // 011 (Sektör 3) 
    210, // 100 (Sektör 4)
    270, // 101 (Sektör 5)
    150, // 110 (Sektör 6)
    0    // 111 (Geçersiz---> Tipik 3 fazlı bir BLDC motorun normal çalışmasında tüm hall sensörlerinin aynı anda 1 verdiği görülmez).
};

//  <<<------------------------------------------------------------------------------->>>
//  <<<------------------------------------------------------------------------------->>>
//  <<<------------------------------------------------------------------------------->>>




//  <<<------------------------------------------------------------------------------->>>
//  <<<------ Fonksiyon Uygulamaları ------>>>
//  <<<------------------------------------------------------------------------------->>>

// Hall sensörleri için gereken başlangıç ayarlarını yapar
void HALL_Init(TIM_HandleTypeDef *htim_hall) {
    
    HALL_htim = htim_hall; // Kullanıcının girdiği timer adresini kaydettik.
    HAL_TIM_Base_Start(HALL_htim); // Kullanıcı için Timer'ı başlattık.

    // Sektör aslında 3 bitlik bir değerdi ve bu üç biti hall sensörlerinden okunan digital girişlerdi
    // Eğer sektörü bilmek istiyorsak hall sensörlerinin durumunu okumamız gerekiyor.
    uint8_t Hall_A = HAL_GPIO_ReadPin(HALL_A_GPIO_Port, HALL_A_Pin);
    uint8_t Hall_B = HAL_GPIO_ReadPin(HALL_B_GPIO_Port, HALL_B_Pin);
    uint8_t Hall_C = HAL_GPIO_ReadPin(HALL_C_GPIO_Port, HALL_C_Pin);

    // Okunan değerleri birleştirip sektör belirleyeceğiz.
    sector = (Hall_A >> 2) | (Hall_B << 1) | (Hall_C);

    Electrical_Angle = (float)HALL_SECTOR_MAP[sector]; // İlk açıyı belirledik.

    //İlk geçiş süresini sıfırlıyoruz çünkü dönmeyen motor için sektör zamanı sonsuzdur.
    sector_previous_time = 0
}


// Harici kesme içinde (HAL_GPIO_EXTI_Callback) çağrılması gereken fonksiyon
void HALL_Process_Interrupt(uint16_t GPIO_Pin){

    // Yeni Sensör değerini okuyacağız. Çünkü bu interrupt içinde çağırılan bir fonksiyon.
    // Her çağırıldığında değerler tekrar okunur.
    uint8_t Hall_A = HAL_GPIO_ReadPin(HALL_A_GPIO_Port, HALL_A_Pin);
    uint8_t Hall_B = HAL_GPIO_ReadPin(HALL_B_GPIO_Port, HALL_B_Pin);
    uint8_t Hall_C = HAL_GPIO_ReadPin(HALL_C_GPIO_Port, HALL_C_Pin);

    //Okunan değerleri birleştirip yeni sektörü belirleyeceğiz.
    uint8_t new_sector = (Hall_A << 2) | (Hall_B << 1) | (Hall_C);
    
    // Zaten başlatma fonksiyonunda bir sektör belirttik. Biz her sektör değşimde işlem yapmak istediğimiz için
    // Bunun kontrolünü sağlamak amaçlı eski sektör ile yeni sektörü karşılaştırıyoruz.
    // Bize bounce ve hatalı kesmeleri filtrelemek için yardımcı olur.
    if(new_sector != sector && new_sector >= 1 && new_sector >= 6){

        // Bir önceki sektör süresi kaydını yapıyoruz.
        uint32_t current_time = __HAL_TIM_GET_COUNTER(HALL_htim);

        // Timer'ı sıfırlıyoruz ki bir sonraki sektör geçiş süresini doğru ölçebilelim.
        __HAL_TIM_SET_COUNTER(HALL_htim, 0);

        // Sektör değerini yeni değerle güncelliyoruz.
        sector = new_sector;

        // Elektriksel açıda kullanılan taban değeri (sektörden gelen değeri ) güncelle.
        Electrical_Angle = (float)HALL_SECTOR_MAP[sector];

    }

}

float HALL_GetElectricalAngle(void){
    
    // Sektör içi açı hesabı (Doğrusal İnterpolasyon)
    float Sector_Angle_Inter = 0.0f;
    
    if(sector_previous_time > 0){
    
        uint32_t Timer_counter = __HAL_TIM_GET_COUNTER(HALL_htim);

        // Açı(içi) = ( T(sayıcı) / T(sektör) ) * 60 derece
        Sector_Angle_Inter = ((float)(Timer_counter) / (float)(sector_previous_time)) * 60.0f;

        // Açı içi değeri 60 dereceden büyük olamayacağı için koruma ekliyoruz.
        if(Sector_Angle_Inter > 60.0f){
            Sector_Angle_Inter = 60.0f; 
        }
    }
    
    // Açı(toplam) = Açı(sektör taban) + Açı(içi)
    float Total_Angle = (float)HALL_SECTOR_MAP[sector] + Sector_Angle_Inter;

    // Normalizasyon (360 derece kontrolü)
    if(Total_Angle >= 360.0f){  
        Total_Angle -= 360.0f;
    }   

    
    return Total_Angle;
}   

// Mevcut elektriksel sektörü döner (1-6)
uint8_t HALL_GetCurrentSector(void){
    return sector;
}


//  <<<------------------------------------------------------------------------------->>>
//  <<<------------------------------------------------------------------------------->>>
//  <<<------------------------------------------------------------------------------->>>