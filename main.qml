import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import filter 1.0

Window {
    visible: true
    width: 720
    height: 540
    title: qsTr("Median Filter")

    Column{
        id: layout
        anchors.leftMargin: 10
        anchors.topMargin: 10
        spacing: 5


        x: 10
        y: 10
        Row{
            spacing: 5
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Image name:")
            }
            TextField {
                id: filename
                implicitWidth: 300
            }
            Button {
                text: qsTr("&Browse...")
                onClicked: filedialog.visible = true

            }
        }
        Row{
            spacing: 5
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Enter N:")
            }
            TextField {
                id: dimension
                implicitWidth: 300
                text: "5"
            }
        }
        Row{
            spacing: 5
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Enter number of threads:")
            }
            TextField {
                id: nthread
                implicitWidth: 300
                text: "1"
            }
        }
        Row{
            spacing: 5
            Image {
                width: 350
                height: 350
                fillMode: Image.PreserveAspectFit
                id: originalImage
                source: "/default.jpg"
            }
            Image {
                id: modifiedImage
                width: 350
                height: 350
                fillMode: Image.PreserveAspectFit
                source: "/default.jpg"
            }
        }

        Row {
            Text {
                text: qsTr("Done(%): ")
            }
            Text {
                id: done
                text: "0"
            }
        }
        Row {
            Text {
                text: qsTr("Time: ")
            }
            Text {
                id: timer
                text: "0"
            }
        }

        Row {
            Button {
                id: start
                text: qsTr("&Start")
                onClicked: filter.start()
            }
            Button {
                id: stop
                text: qsTr("&Stop")
                onClicked: filter.stop()
            }
        }


    }
    Filter {
        id: filter
        filename: filename.text
        rad: parseInt(dimension.text)
        numthread:  parseInt(nthread.text)
        onFilteringDone: {modifiedImage.source = "image://filterimg/myImage?id=" + Math.random();
            timer.text =t;}
        onPixelDone: done.text=n;
    }
    FileDialog {
        id: filedialog
        title: qsTr("Select a file")
        selectMultiple: false
        selectFolder: false
        nameFilters: [ "Image files (*.jpg *.png)" ]
        selectedNameFilter: "Image files (*.jpg *.png)"
        onAccepted: {
            originalImage.source = fileUrl;
            filename.text = fileUrl.toString().replace("file://", "");
        }
    }
}
