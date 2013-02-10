#ifndef ICQCONTACT_H
#define ICQCONTACT_H

#include <QByteArray>
#include <QString>

#include "contacts/contact.h"
#include "contacts/imcontact.h"
#include "clients/client.h"
#include "icqstatus.h"
#include "icq_defines.h"

class ICQClient;

class ICQ_EXPORT ICQContact : public QObject, public SIM::IMContact
{
    Q_OBJECT
public:
    ICQContact(ICQClient* client);

    virtual QString name() const;
    virtual void setParentContactId(int contactId);
    virtual int parentContactId() const;
    virtual SIM::IMContactId id() const;

    void setMetaContact(SIM::Contact* c);
    int metaContactId() const;

    virtual SIM::Client* client();
    virtual SIM::IMStatusPtr status() const;
    void setIcqStatus(const ICQStatusPtr& s);
    virtual ICQStatusPtr icqStatus() const;

    virtual bool sendMessage(const SIM::MessagePtr& message);
    virtual bool hasUnreadMessages();

    virtual SIM::MessagePtr dequeueUnreadMessage();
    virtual void enqueueUnreadMessage(const SIM::MessagePtr& message);

    virtual SIM::IMGroupWeakPtr group();

    virtual QString makeToolTipText();

    virtual QList<SIM::MessageTypeDescriptor> allMessageTypes() const;

    virtual void serialize(QDomElement& element);
    virtual void deserialize(QDomElement& element);
    virtual bool deserialize(const QString& data);

    virtual SIM::PropertyHubPtr saveState();
    virtual bool loadState(SIM::PropertyHubPtr state);

    unsigned long getUin() const;
    void setUin(unsigned long uin);

    QString getScreen() const;
    void setScreen(const QString& screen);

    QString getAlias() const { return m_alias; }
    void setAlias(const QString& alias) { m_alias = alias; }

    QString getCellular() const { return m_cellular; }
    void setCellular(const QString& cellular) { m_cellular = cellular; }

//    unsigned long getStatus() const { return m_status; }
//    void setStatus(unsigned long status) { m_status = status; }

    QString getEncoding() const { return m_encoding; }
    void setEncoding(const QString& encoding) { m_encoding = encoding; }

    unsigned long getClass() const { return m_class; }
    void setClass(unsigned long cl) { m_class = cl; }

    unsigned int getStatusTime() const { return m_statusTime; }
    void setStatusTime(unsigned long statusTime) { m_statusTime = statusTime; }

    unsigned int getOnlineTime() const { return m_onlineTime; }
    void setOnlineTime(unsigned long onlineTime) { m_onlineTime = onlineTime; }

    unsigned int getWarningLevel() const { return m_warningLevel; }
    void setWarningLevel(unsigned long warningLevel) { m_warningLevel = warningLevel; }

    unsigned long getIP() const { return m_ip; }
    void setIP(unsigned long ip) { m_ip = ip; }

    unsigned long getRealIP() const { return m_realip; }
    void setRealIP(unsigned long ip) { m_realip = ip; }

    unsigned int getPort() const { return m_port; }
    void setPort(unsigned int port) { m_port = port; }

    unsigned long getDCcookie() const { return m_dcCookie; }
    void setDCcookie(unsigned long cookie) { m_dcCookie = cookie; }

    unsigned long getCaps() const { return m_caps; }
    void setCaps(unsigned long caps) { m_caps = caps; }

    unsigned long getCaps2() const { return m_caps2; }
    void setCaps2(unsigned long caps) { m_caps2 = caps; }

    QString getAutoReply() const { return m_autoReply; }
    void setAutoReply(const QString& autoreply) { m_autoReply = autoreply; }

    unsigned long getIcqID() const { return m_icqId; }
    void setIcqID(unsigned long id) { m_icqId = id; }

    bool isChecked() const { return m_checked; }
    void setChecked(bool c) { m_checked = c; }

    unsigned long getGrpID() const { return m_grpId; }
    void setGrpID(unsigned long id) { m_grpId = id; }

    unsigned long getIgnoreId() const { return m_ignoreId; }
    void setIgnoreId(unsigned long id) { m_ignoreId = id; }

    unsigned long getVisibleId() const { return m_visibleId; }
    void setVisibleId(unsigned long id) { m_visibleId = id; }

    unsigned long getInvisibleId() const { return m_invisibleId; }
    void setInvisibleId(unsigned long id) { m_invisibleId = id; }

    unsigned long getContactVisibleId() const { return m_contactVisibleId; }
    void setContactVisibleId(unsigned long id) { m_contactVisibleId = id; }

    unsigned long getContactInvisibleId() const { return m_contactInvisibleId; }
    void setContactInvisibleId(unsigned long id) { m_contactInvisibleId = id; }

    bool getWaitAuth() const { return m_waitAuth; }
    void setWaitAuth(bool w) { m_waitAuth = w; }

    bool getWantAuth() const { return m_wantAuth; }
    void setWantAuth(bool w) { m_wantAuth = w; }

    bool getWebAware() const { return m_webAware; }
    void setWebAware(bool w) { m_webAware = w; }

    unsigned long getInfoUpdateTime() const { return m_infoUpdateTime; }
    void setInfoUpdateTime(unsigned long t) { m_infoUpdateTime = t; }

    unsigned long getPluginInfoTime() const { return m_pluginInfoTime; }
    void setPluginInfoTime(unsigned long t) { m_pluginInfoTime = t; }

    unsigned long getPluginStatusTime() const { return m_pluginStatusTime; }
    void setPluginStatusTime(unsigned long t) { m_pluginStatusTime = t; }

    unsigned long getInfoFetchTime() const { return m_infoFetchTime; }
    void setInfoFetchTime(unsigned long t) { m_infoFetchTime = t; }

    unsigned long getPluginInfoFetchTime() const { return m_pluginInfoFetchTime; }
    void setPluginInfoFetchTime(unsigned long t) { m_pluginInfoFetchTime = t; }

    unsigned long getPluginStatusFetchTime() const { return m_pluginStatusFetchTime; }
    void setPluginStatusFetchTime(unsigned long t) { m_pluginStatusFetchTime = t; }

    unsigned int getMode() const { return m_mode; }
    void setMode(unsigned int mode) { m_mode = mode; }

    unsigned int getVersion() const { return m_version; }
    void setVersion(unsigned int version) { m_version = version; }

    unsigned int getBuild() const { return m_build; }
    void setBuild(unsigned int build) { m_build = build; }

    QString getNick() const { return m_nick; }
    void setNick(const QString& nick) { m_nick = nick; }

    QString getFirstName() const { return m_firstName; }
    void setFirstName(const QString& firstName) { m_firstName = firstName; }

    QString getLastName() const { return m_lastName; }
    void setLastName(const QString& lastName) { m_lastName = lastName; }

    QString getMiddleName() const { return m_middleName; }
    void setMiddleName(const QString& middlename) { m_middleName = middlename; }

    QString getMaiden() const { return m_maiden; }
    void setMaiden(const QString& maiden) { m_maiden = maiden; }

    QString getEmail() const { return m_email; }
    void setEmail(const QString& email) { m_email = email; }

    bool getHiddenEmail() const { return m_hiddenEmail; }
    void setHiddenEmail(bool b) { m_hiddenEmail = b; }

    QString getCity() const { return m_city; }
    void setCity(const QString& city) { m_city = city; }

    QString getState() const { return m_state; }
    void setState(const QString& state) { m_state = state; }

    QString getHomePhone() const { return m_homephone; }
    void setHomePhone(const QString& homephone) { m_homephone = homephone; }

    QString getHomeFax() const { return m_homefax; }
    void setHomeFax(const QString& homefax) { m_homefax = homefax; }

    QString getAddress() const { return m_address; }
    void setAddress(const QString& address) { m_address = address; }

    QString getPrivateCellular() const { return m_privateCellular; }
    void setPrivateCellular(const QString& cellular) { m_privateCellular = cellular; }

    QString getZip() const { return m_zip; }
    void setZip(const QString& zip) { m_zip = zip; }

    int getCountry() const { return m_country; }
    void setCountry(unsigned long country) { m_country =  country; }

    unsigned long getTimeZone() const { return m_timezone; }
    void setTimeZone(unsigned long timezone) { m_timezone = timezone; }

    int getAge() const { return m_age; }
    void setAge(int age) { m_age = age; }

    int getGender() const { return m_gender; }
    void setGender(unsigned long gender) { m_gender = gender; }

    QString getHomepage() const { return m_homepage; }
    void setHomepage(const QString& homepage) { m_homepage = homepage; }

    QDate getBirthday() const { return m_birthday; }
    void setBirthday(const QDate& date) { m_birthday = date; }

    int getPrimaryLanguage() const { return m_primaryLanguage; }
    void setPrimaryLanguage(int language) { m_primaryLanguage = language; }

    int getSecondaryLanguage() const { return m_secondaryLanguage; }
    void setSecondaryLanguage(int language) { m_secondaryLanguage = language; }

    int getTertiaryLanguage() const { return m_tertiaryLanguage; }
    void setTertiaryLanguage(int language) { m_tertiaryLanguage = language; }

    QString getEMails() const { return m_emails; }
    void setEMails(const QString& emails) { m_emails = emails; }

    QString getWorkCity() const { return m_workcity; }
    void setWorkCity(const QString& workcity) { m_workcity = workcity; }

    QString getWorkState() const { return m_workstate; }
    void setWorkState(const QString& workstate) { m_workstate = workstate; }

    QString getWorkPhone() const { return m_workphone; }
    void setWorkPhone(const QString& workphone) { m_workphone = workphone; }

    QString getWorkFax() const { return m_workfax; }
    void setWorkFax(const QString& workfax) { m_workfax = workfax; }

    QString getWorkAddress() const { return m_workaddress; }
    void setWorkAddress(const QString& workaddress) { m_workaddress = workaddress; }

    QString getWorkZip() const { return m_workzip; }
    void setWorkZip(const QString& workzip) { m_workzip = workzip; }

    int getWorkCountry() const { return m_workcountry; }
    void setWorkCountry(int workcountry) { m_workcountry = workcountry; }

    QString getWorkName() const { return m_workname; }
    void setWorkName(const QString& workname) { m_workname = workname; }

    QString getWorkDepartment() const { return m_workdepartment; }
    void setWorkDepartment(const QString& workdepartment) { m_workdepartment = workdepartment; }

    QString getWorkPosition() const { return m_workposition; }
    void setWorkPosition(const QString& workposition) { m_workposition = workposition; }

    int getOccupation() const { return m_occupation; }
    void setOccupation(int occupation) { m_occupation = occupation; }

    QString getWorkHomepage() const { return m_workhomepage; }
    void setWorkHomepage(const QString& homepage) { m_workhomepage = homepage; }

    QString getAbout() const { return m_about; }
    void setAbout(const QString& about) { m_about = about; }

    int getInterest(int interestNum) const { return m_interestsCode[interestNum]; }
    QString getInterestText(int interestNum) const { return m_interests[interestNum]; }
    void setInterest(int interestNum, int interestCode, const QString& interestText)
    {
        Q_ASSERT((interestNum >= 0) && (interestNum < 4));
        m_interestsCode[interestNum] = interestCode;
        m_interests[interestNum] = interestText;
    }

    int getBackgroundCode(int backgroundNum) const
    {
        Q_ASSERT((backgroundNum >= 0) && (backgroundNum < 3));
        return m_backgroundsCode[backgroundNum];
    }

    QString getBackgroundText(int backgroundNum) const
    {
        Q_ASSERT((backgroundNum >= 0) && (backgroundNum < 3));
        return m_backgroundsText[backgroundNum];
    }

    void setBackground(int backgroundNum, int backgroundCode, const QString& backgroundText)
    {
        Q_ASSERT((backgroundNum >= 0) && (backgroundNum < 3));
        m_backgroundsCode[backgroundNum] = backgroundCode;
        m_backgroundsText[backgroundNum] = backgroundText;
    }

    int getAffiliationCode(int affiliationNum) const
    {
        Q_ASSERT((affiliationNum >= 0) && (affiliationNum < 3));
        return m_affiliationsCode[affiliationNum];
    }

    QString getAffiliationText(int affiliationNum) const
    {
        Q_ASSERT((affiliationNum >= 0) && (affiliationNum < 3));
        return m_affiliationsText[affiliationNum];
    }

    void setAffiliation(int affiliationNum, int affiliationCode, const QString& affiliationText)
    {
        Q_ASSERT((affiliationNum >= 0) && (affiliationNum < 3));
        m_affiliationsCode[affiliationNum] = affiliationCode;
        m_affiliationsText[affiliationNum] = affiliationText;
    }

    unsigned long getFollowMe() const { return m_followme; }
    void setFollowMe(unsigned long followme) { m_followme = followme; }

    bool getSharedFiles() const { return m_sharedFiles; }
    void setSharedFiles(bool b) { m_sharedFiles = b; }

    unsigned long getICQPhone() const { return m_icqPhone; }
    void setICQPhone(unsigned long phone) { m_icqPhone = phone; }

    QString getPicture() const { return m_picture; }
    void setPicture(const QString& picture) { m_picture = picture; }

    unsigned long getPictureWidth() const { return m_pictureSize.width(); }
    void setPictureWidth(unsigned long width) { m_pictureSize.setWidth(width); }

    unsigned long getPictureHeight() const { return m_pictureSize.height(); }
    void setPictureHeight(unsigned long height) { m_pictureSize.setHeight(height); }

    QString getPhoneBook() const { return m_phoneBook; }
    void setPhoneBook(const QString& phonebook) { m_phoneBook = phonebook; }

    bool getProfileFetch() const { return m_profileFetch; }
    void setProfileFetch(bool pf) { m_profileFetch = pf; }

    bool getTyping() const { return m_typing; }
    void setTyping(bool t) { m_typing = t; }

    bool getBadClient() const { return m_badClient; }
    void setBadClient(bool bc) { m_badClient = bc; }

    bool getNoDirect() const { return m_noDirect; }
    void setNoDirect(bool nd) { m_noDirect = nd; }

    bool getInvisible() const { return m_invisible; }
    void setInvisible(bool i) { m_invisible = i; }

    unsigned long getBuddyRosterID() const { return m_buddyRosterId; }
    void setBuddyRosterID(unsigned long id) { m_buddyRosterId = id; }

    unsigned long getBuddyID() const { return m_buddyId; }
    void setBuddyID(unsigned long id) { m_buddyId = id; }

    QByteArray getBuddyHash() const { return m_buddyHash; }
    void setBuddyHash(const QByteArray& arr) { m_buddyHash = arr; }

    QByteArray getUnknown(int i) const { return m_unknown[i]; }
    void setUnknown(int i, const QByteArray& arr) { m_unknown[i] = arr; }

    QObject* getDirect() const { return m_direct; }
    void setDirect(QObject* obj) { m_direct = obj; }

    QObject* getDirectPluginInfo() const { return m_directPluginInfo; }
    void setDirectPluginInfo(QObject* obj) { m_directPluginInfo = obj; }

    QObject* getDirectPluginStatus() const { return m_directPluginStatus; }
    void setDirectPluginStatus(QObject* obj) { m_directPluginStatus = obj; }

    QByteArray getAvatarHash() const { return m_avatarHash; }
    void setAvatarHash(const QByteArray& arr) { m_avatarHash = arr; }

    void deserializeLine(const QString& key, const QString& value);

public slots:
    void statusChanged();

private:
    unsigned long m_uin;
    QString m_screen;
    QString m_alias;
    QString m_cellular;

    QString m_encoding;
    unsigned long m_status;
    unsigned long m_class;
    unsigned int m_statusTime;
    unsigned int m_onlineTime;
    unsigned int m_warningLevel;
    unsigned long m_ip;
    unsigned long m_realip;
    unsigned int m_port;
    unsigned long m_dcCookie;
    unsigned long m_caps;
    unsigned long m_caps2;
    QString m_autoReply;
    unsigned long m_icqId;
    bool m_checked;
    unsigned long m_grpId;
    unsigned long m_ignoreId;
    unsigned long m_visibleId;
    unsigned long m_invisibleId;
    unsigned long m_contactVisibleId;
    unsigned long m_contactInvisibleId;
    bool m_waitAuth;
    bool m_wantAuth;
    bool m_webAware;
    unsigned long m_infoUpdateTime;
    unsigned long m_pluginInfoTime;
    unsigned long m_pluginStatusTime;
    unsigned long m_infoFetchTime;
    unsigned long m_pluginInfoFetchTime;
    unsigned long m_pluginStatusFetchTime;
    unsigned int m_mode;
    unsigned int m_version;
    unsigned int m_build;
    QString m_nick;
    QString m_firstName;
    QString m_lastName;
    QString m_middleName;
    QString m_maiden;
    QString m_email;
    bool m_hiddenEmail;
    QString m_city;
    QString m_state;
    QString m_homephone;
    QString m_homefax;
    QString m_address;
    QString m_privateCellular;
    QString m_zip;
    int m_country;
    unsigned long m_timezone;
    int m_age;
    int m_gender;
    QString m_homepage;
    QDate m_birthday;
    int m_primaryLanguage;
    int m_secondaryLanguage;
    int m_tertiaryLanguage;
    QString m_emails;
    QString m_workcity;
    QString m_workstate;
    QString m_workphone;
    QString m_workfax;
    QString m_workaddress;
    QString m_workzip;
    int m_workcountry;
    QString m_workname;
    QString m_workdepartment;
    QString m_workposition;
    int m_occupation;
    QString m_workhomepage;
    QString m_about;

    int m_interestsCode[4];
    QString m_interests[4];

    int m_backgroundsCode[3];
    QString m_backgroundsText[3];

    int m_affiliationsCode[3];
    QString m_affiliationsText[3];
    unsigned long m_followme;
    bool m_sharedFiles;
    unsigned long m_icqPhone;
    QString m_picture;
    QSize m_pictureSize;
    QString m_phoneBook;
    bool m_profileFetch;
    bool m_typing;
    bool m_badClient;
    bool m_noDirect;
    bool m_invisible;
    unsigned long m_buddyRosterId;
    unsigned long m_buddyId;
    QByteArray m_buddyHash;
    QObject* m_direct;
    QObject* m_directPluginInfo;
    QObject* m_directPluginStatus;
    QByteArray m_unknown[6];

    QByteArray m_avatarHash;

    ICQClient* m_client;
    ICQStatusPtr m_icqstatus;
    SIM::Contact* m_metaContact;

    int m_contactId;
};

typedef QSharedPointer<ICQContact> ICQContactPtr;

#endif // ICQCONTACT_H
