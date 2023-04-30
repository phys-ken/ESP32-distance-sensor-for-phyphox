#include <phyphoxBle.h>
#include <Wire.h>

// ピン設定
int Echo = 14; // Echoピン
int Trig = 15; // Trigピン
// 平滑化のための移動平均フィルターのサイズ
const int filterSize = 10;
// 移動平均フィルターのバッファ
float filterBuffer[filterSize];
// 移動平均フィルターの現在のインデックス
int filterIndex = 0;


// 距離の測定関数の定義（400mm以上は前回のセンサー値を返す）
float readHCSR04cm()
{
  static float lastDstCMs = 0.0; // 前回の距離を保持するための変数

  // trigger
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  // echo
  // pulseIn()関数でエコーピンの信号を計測する（タイムアウト時間：20ミリ秒）
  int intervalUS = pulseIn(Echo, HIGH, 20000) / 2; // 超音波の往復の時間を2で割って片道の時間（単位：μsec）にする

  // 時間を距離に変換
  // 音速：340m/s(15℃)
  float dstM = (float)intervalUS * 340.0 / 1000.0 / 1000.0; // センサー出力の値（単位：m）
  float dstCMs = dstM * 100.0;                              // センサー出力の値（単位：cm）

  // 移動平均フィルターをかける
  filterBuffer[filterIndex] = dstCMs;
  filterIndex = (filterIndex + 1) % filterSize;
  float filteredDist = 0.0;
  for (int i = 0; i < filterSize; i++) {
    filteredDist += filterBuffer[i];
  }
  filteredDist /= filterSize;

  // 距離が400mmを超えたら前回の距離を返す
  if (filteredDist > 400.0) {
    return lastDstCMs;
  } else {
    lastDstCMs = filteredDist;
    return filteredDist;
  }
}

void setup() {
  Serial.begin(9600);
  PhyphoxBLE::start("距離センサ001");                //Start the BLE server
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);


  //Experiment
  PhyphoxBleExperiment experiment;

  experiment.setTitle("x-tグラフ");
  experiment.setCategory("距離センサ001");
  experiment.setDescription("Plot the distance from a time-of-flight sensor over time.");

  //View
  PhyphoxBleExperiment::View view;

  //Graph
  PhyphoxBleExperiment::Graph graph;
  graph.setLabel("x-tグラフ");
  graph.setUnitX("s");
  graph.setUnitY("cm");
  graph.setLabelX("時刻");
  graph.setLabelY("距離");

  graph.setChannel(0, 1);

  //Second Graph
  PhyphoxBleExperiment::Graph secondGraph;      //Create graph which will plot random numbers over time
  secondGraph.setLabel("v-tグラフ");
  secondGraph.setUnitX("s");
  secondGraph.setUnitY("cm/s");
  secondGraph.setLabelX("時刻");
  secondGraph.setLabelY("速度");


  secondGraph.setChannel(0, 2);

  view.addElement(graph);                 //Attach graph to view
  view.addElement(secondGraph);
  experiment.addView(view);               //Attach view to experiment
  PhyphoxBLE::addExperiment(experiment);  //Attach experiment to server


}

void loop() {
  //uncomment next line if using senseBox MCU or Arduino Nano 33 IoT
  //PhyphoxBLE::poll();
  float dist = readHCSR04cm();
  float v = dist / 2;
  PhyphoxBLE::write(dist, v);   //Send value to phyphox
  Serial.println(String(dist) + ",,,,," + String(v));
  delay(20);                          //Shortly pause before repeating
}
