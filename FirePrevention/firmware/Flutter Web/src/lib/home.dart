import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';

DatabaseReference db = FirebaseDatabase.instance.reference();

// ignore: must_be_immutable
class HomeScreen extends StatefulWidget {

  HomeScreen({Key key}) : super(key: key);

  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    double h = MediaQuery.of(context).size.height;
    double w = MediaQuery.of(context).size.width;

    return Scaffold(
      appBar: AppBar(
        title: Text("Fire Prevention Dashboard"),
      ),
      body: Padding(
        padding: const EdgeInsets.only(bottom: 50),
        child: StreamBuilder (
          stream:  db.child('/data').onValue,
          builder: (context, AsyncSnapshot<Event> snapshot) {
            if (snapshot.hasData) {
              Map<dynamic, dynamic> data = snapshot.data.snapshot.value;
              if (data != null){

                return ListView.builder(
                  shrinkWrap: true,
                  physics: BouncingScrollPhysics(),
                  itemCount: data.length, // 6
                  itemBuilder: (context, index) {
                    double temp = data['temp'];
                    double flame = data['flame'];
                    double gas = data['gas'];
                    int alarm = data['alarm'];

                    return ListView(
                      cacheExtent: 1000,
                      shrinkWrap: true,
                      physics: BouncingScrollPhysics(),
                      children: [

                        Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [

                            // if there is no alarm
                            alarm == 0 ?
                            Container(
                              margin: EdgeInsets.only(top: 30),
                              padding: EdgeInsets.symmetric(vertical: 20, horizontal: 40),
                              decoration: BoxDecoration(
                                  borderRadius: BorderRadius.circular(5),
                                  color: Colors.greenAccent,
                                  boxShadow: [
                                    BoxShadow(
                                        color: Colors.black38,
                                        blurRadius: 2)
                                  ]),
                              child: Row(
                                mainAxisAlignment: MainAxisAlignment.center,
                                children: [
                                  Text("SAFE. No Fire Detected."),
                                ],
                              ),
                            ) :

                            // if there is alarm
                            Container(
                              margin: EdgeInsets.only(top: 30),
                              padding: EdgeInsets.symmetric(vertical: 20, horizontal: 40),
                              decoration: BoxDecoration(
                                  borderRadius: BorderRadius.circular(5),
                                  color: Colors.red,
                                  boxShadow: [
                                    BoxShadow(
                                        color: Colors.black38,
                                        blurRadius: 2)
                                  ]),
                              child: Row(
                                mainAxisAlignment: MainAxisAlignment.center,
                                children: [
                                  Icon(Icons.notifications_active_rounded, size: 50,),
                                  SizedBox(width: 50,),
                                  Text("FIRE DETECTED! DO SOMETHING ASAP..."),
                                  SizedBox(width: 50,),
                                  Icon(Icons.local_fire_department, size: 50,),
                                ],
                              ),
                            ),
                          ],
                        ),

                        // Gauges to show the sensor data
                        Container(
                            margin: EdgeInsets.symmetric(vertical: 50, horizontal: 50),
                            decoration: BoxDecoration(
                                borderRadius: BorderRadius.circular(10),
                                color: Colors.white,
                                boxShadow: [
                                  BoxShadow(
                                      color: Colors.black38,
                                      blurRadius: 4)
                                ]),
                            child: Center(
                                child: Row(
                                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                                  children: [
                                    Container(
                                        width: w*0.25,
                                        child: SfRadialGauge(
                                            title: GaugeTitle(text: "Temperature"),
                                            axes: <RadialAxis>[
                                              RadialAxis(minimum: 0, maximum: 200,
                                                  ranges: <GaugeRange>[
                                                    GaugeRange(startValue: 0, endValue: 57, color:Colors.greenAccent),
                                                    GaugeRange(startValue: 57,endValue: 200,color: Colors.red)],
                                                  pointers: <GaugePointer>[
                                                    NeedlePointer(value: temp)],
                                                  annotations: <GaugeAnnotation>[
                                                    GaugeAnnotation(widget: Container(child:
                                                    Text('90.0',style: TextStyle(fontSize: 25,fontWeight: FontWeight.bold))),
                                                        angle: 90, positionFactor: 0.5
                                                    )]
                                              )])
                                    ),
                                    Container(
                                        width: w*0.25,
                                        padding: EdgeInsets.symmetric(vertical: 25),
                                        child: SfRadialGauge(
                                            title: GaugeTitle(text: "Smoke/Gas Detector"),
                                            axes: <RadialAxis>[
                                              RadialAxis(minimum: 0, maximum: 1023,
                                                  ranges: <GaugeRange>[
                                                    GaugeRange(startValue: 0, endValue: 1000, color:Colors.greenAccent),
                                                    GaugeRange(startValue: 1001,endValue: 1023,color: Colors.red)],
                                                  pointers: <GaugePointer>[
                                                    NeedlePointer(value: gas)],
                                                  annotations: <GaugeAnnotation>[
                                                    GaugeAnnotation(widget: Container(child:
                                                    Text('90.0',style: TextStyle(fontSize: 25,fontWeight: FontWeight.bold))),
                                                        angle: 90, positionFactor: 0.5
                                                    )]
                                              )])
                                    ),
                                    Container(
                                        width: w*0.25,
                                        child: SfRadialGauge(
                                            title: GaugeTitle(text: "Flame Detector"),
                                            axes: <RadialAxis>[
                                              RadialAxis(minimum: 0, maximum: 1023,
                                                  ranges: <GaugeRange>[
                                                    GaugeRange(startValue: 0, endValue: 999, color:Colors.red),
                                                    GaugeRange(startValue: 999,endValue: 1023,color: Colors.greenAccent)],
                                                  pointers: <GaugePointer>[
                                                    NeedlePointer(value: flame)],
                                                  annotations: <GaugeAnnotation>[
                                                    GaugeAnnotation(widget: Container(child:
                                                    Text('90.0',style: TextStyle(fontSize: 25,fontWeight: FontWeight.bold))),
                                                        angle: 90, positionFactor: 0.5
                                                    )]
                                              )])
                                    ),
                                  ],
                                ))),
                      ],
                    );
                  },
                );
              }
              // if the database has no sensor data
              else{
                return Center(child: Text("No Data!"),);
              }
            }
            // if there was an error getting data from database
            else if (snapshot.hasError) {
              return Center(child: Text("Some Error Occurred!"),);
            }
            return Center(child: CupertinoActivityIndicator(radius: 20,));
          },
        ),
      ),
    );
  }
}
