import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

Window {
    id:window
    visible: true
    width: 400
    height: 720
    minimumWidth: 400
    minimumHeight: 720
    title: "svcar:  [" + (Account.connected ? "connected" : "disconnected") + "]"

    Rectangle{
        anchors.fill: parent
        NaviBar{
            id:bar
            width: horizontal ? window.width : window.width * 0.2
            height: horizontal ? window.height * 0.2 : window.height
            horizontal: window.width < window.height
            anchors.bottom: parent.bottom
            onNaviTo: {
                if(page === "me")               pageContainer.source = Account.islogin ? "MePage.qml" : "LoginPage.qml"
                else if(page === "car")         pageContainer.source = "CarPage.qml"
                else if(page === "pc")          pageContainer.source = "PCPage.qml"
                else if(page === "cellphone")   pageContainer.source = "HandledPage.qml"
                else if(page === "message")     pageContainer.source = "MessagePage.qml"
                else if(page === "contacts")    pageContainer.source = "ContactsPage.qml"
            }
        }

        Item{
            width: bar.horizontal ? window.width : window.width - bar.width
            height: bar.horizontal ? window.height - bar.height : window.height
            anchors.left: bar.horizontal ? parent.left : bar.right

            Loader{
                id:pageContainer
                width: parent.width
                height: parent.height
                source: "LoginPage.qml"
            }
            Connections{
                target: pageContainer.item
                onNaviToLogin: pageContainer.source = "LoginPage.qml"
                onNaviToRegist: pageContainer.source = "RegistPage.qml"
                onLoginResult: if(success) pageContainer.source = "MePage.qml"
                onLogout: pageContainer.source = "LoginPage.qml"
                onEnterChat: {pageContainer.source = "ChatPage.qml"}
            }

            Image{
                width:parent.width
                height: parent.height
                source: "images/bg.jpg"
                opacity: 0.3
                visible: false
            }
         //   RegistPage{ onNaviToLogin: pageContainer.switchPage(6) }
         //   LoginPage{onNaviToRegist: pageContainer.switchPage(7); onNaviToMe: pageContainer.switchPage(6)}
          //  MePage{onNaviToLogin: pageContainer.switchPage(6)}

        }
    }


/*
    Item
    {
        id:pages
        anchors.left: bar.right
        width: w.width - bar.width
        height: w.height
        Image{
            width:parent.width
            height: parent.height
            source: "img/picSelect/Lnk/1.jpg"
            opacity: 0.3
        }

        CarPage{ id:carPg; visible: false }
        PCPage{ id:pcpg; visible: false }
        HandledPage{ id:handledPg; visible:false }
        MessagePage{ id:msgPg; visible: false }
        ContactsPage{ id:contactsPg; visible: false }
        /////
        LoginPage{ id:loginPg; visible: false; onSwitchPage: pages.switchPage(page) }
        RegistPage{ id:regPg; visible: false; onSwitchPage: pages.switchPage(page) }
        MyAccountPage{ id:myaccountPg; visible: false; onSwitchPage: pages.switchPage(page) }

        function switchPage(page)
        {
            carPg.visible = false
            pcpg.visible = false
            handledPg.visible = false
            msgPg.visible = false
            contactsPg.visible = false
            regPg.visible = false
            loginPg.visible = false
            myaccountPg.visible = false

            if(page === 0)          carPg.visible = true
            else if(page === 1)     pcpg.visible = true
            else if(page === 2)     handledPg.visible = true
            else if(page === 3)     msgPg.visible = true
            else if(page === 4)     contactsPg.visible = true
            else if(page === 5)     regPg.visible = true
            else if(page === 6)     loginPg.visible = true
            else if(page === 7)     myaccountPg.visible = true
        }
    }

    Component.onCompleted: {
        pages.switchPage(6)
    }

    Component.onDestruction: Account.logout(Account.userID, Account.password)
*/
    Component.onDestruction: Account.logout(Account.userID, Account.password)
}
