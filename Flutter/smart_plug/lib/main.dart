/**
 * Author: Hrvoje Dumancic
 * Last updated: 21.7.2020.
 */

import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        primarySwatch: Colors.blue,

        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      home: MyHomePage(
          title: 'Smart plug',
      ),
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key key, this.title}) : super(key: key);

  final String title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  bool socketOnState = false;
  String socketCurrentState = "";

  bool getSocketValue(String value) {
    if (value == "true") {
      socketCurrentState = "on";
      return true;
    }
    socketCurrentState = "off";
    return false;
  }

  void postSocketValue() {
    bool value = true;
    if (socketOnState) {
      value = false;
    }

    FirebaseDatabase.instance.reference().child("1").child("OnOff").child("on").set(value);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: new StreamBuilder(
          stream: FirebaseDatabase.instance.reference().child("1").child("OnOff").child("on").onValue,
          builder: (BuildContext context, AsyncSnapshot<Event> event) {
            if (event.hasData)
              socketOnState = getSocketValue(event.data.snapshot.value.toString());
              return Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  crossAxisAlignment: CrossAxisAlignment.center,
                  children: <Widget>[
                    Center(
                        child:
                        Text("Socket is " + socketCurrentState,
                            style: TextStyle(fontSize: 35))
                    ),
                    SizedBox(height: 30),
                    RaisedButton(
                        onPressed: postSocketValue,
                        child: const Text(
                            'Change socket state',
                            style: TextStyle(fontSize: 30)
                        )
                    )
                  ]
              );
            return Center(child: new Text('No data in database'));
          }),
    );
  }
}
