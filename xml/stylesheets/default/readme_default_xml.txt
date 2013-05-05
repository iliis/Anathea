Beim erstellen eines neuen Widgets wird jeweils Widget::set(ptree n) aufgerufen,
wobei die Daten aus dem passenden (dessen Namen dem Typ Widget::getType()
entspricht) XML ausgelesen wird.

Es wird immer mindestens Widget.xml übergeben. Priorität haben Werte im
eigentlichen XML, danach die aus <include>-Direktiven und zuletzt
Widget.xml
