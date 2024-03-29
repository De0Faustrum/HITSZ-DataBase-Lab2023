/*==============================================================*/
/* DBMS name:      MySQL 5.0                                    */
/* Created on:     2023/11/2 23:51:21                           */
/*==============================================================*/

DROP SCHEMA IF EXISTS PetAdoptSystem;
CREATE SCHEMA PetAdoptSystem;
USE PetAdoptSystem;

create table AdoptApplication
(
   applicationID        int not null Unique,
   userAccount          int not null References UserInfo(userAccount) On Delete Cascade,
   petID                int not null References pet(petID) On Delete Cascade,
   adoptID              int References AdoptInfo(adoptID) On Delete Cascade,
   adopterName          char(32) not null,
   adopterGender        char(8) not null Constraint cstgen Check(adopterGender = 'Male' Or adopterGender = 'Female'),
   adopterAge           int Constraint cstage Check(adopterAge > 0 And adopterAge < 150),
   adopterCityID        int not null,
   adopterTEL           char(32) not null,
   adopterMail          char(64),
   isApproved           bool not null,
   primary key (applicationID)
);

/*==============================================================*/
/* Table: AdoptInfo                                             */
/*==============================================================*/
create table AdoptInfo
(
   adoptID              int not null unique,
   applicationID        int not null References AdoptApplication(applicationID) On Delete Cascade,
   petID                int not null References pet(petID) On Delete Cascade,
   adoptDate            date Default NOW() ,
   adoptTime            int Default 0,
   primary key (adoptID)
);

/*==============================================================*/
/* Table: Breed                                                 */
/*==============================================================*/
create table Breed
(
   breedID              int not null,
   breedName            char(64),
   primary key (breedID)
);

/*==============================================================*/
/* Table: City                                                  */
/*==============================================================*/
create table City
(
   cityID               int not null,
   cityName             char(32),
   provinceID           int not null References Province(provinceID) On Delete Cascade,
   primary key (cityID)
);

/*==============================================================*/
/* Table: Pet                                                   */
/*==============================================================*/
create table Pet
(
   petID                int not null Unique,
   petName              char(32),
   birthday             date,
   age                  int Constraint cstpage Check(age > 0 And age < 50),
   gender               char(8) Constraint cstsex Check(gender = 'Male' Or gender = 'Female'),
   breedID              int not null,
   adoptID              int References AdoptInfo(adoptID) On Delete Cascade,
   colour               char(20),
   petCharacter         char(20),
   isAdopted            bool not null,
   isApplied            bool not null,
   primary key (petID)
);

/*==============================================================*/
/* Table: RevisitRecord                                         */
/*==============================================================*/
create table RevisitRecord
(
   revisitID            int not null,
   adoptID              int not null References AdoptInfo(adoptID) On Delete Cascade,
   revisitComment       char(255),
   revisitDate          date,
   petID                int not null,
   primary key (revisitID)
);

/*==============================================================*/
/* Table: UserInfo                                              */
/*==============================================================*/
create table UserInfo
(
   userAccount          int not null,
   cityID               int not null,
   userName             char(32) not null,
   userPassword         char(32) not null,
   userPrivilege        char(20) Constraint cstprlg Check(userPrivilege = 'Administator' Or userPrivilege = 'Client'),
   userStatus           char(20) Constraint cststt Check(userStatus = 'Normal' Or userStatus = 'Frozen'),
   primary key (userAccount)
);

/*==============================================================*/
/* Table: province                                              */
/*==============================================================*/
create table province
(
   provinceID           int not null,
   provinceName         char(32),
   primary key (provinceID)
);

/*==============================================================*/
/* Table: Adopter_Pet                                           */
/*==============================================================*/
Create View Adopter_Pet(adopterName, petName
)
As Select
	AdoptApplication.adopterName,
	pet.petName
	From 
		AdoptApplication,
		pet
	Where 
		pet.petID = AdoptApplication.petID
        And	AdoptApplication.isApproved = true;

/*==============================================================*/
/* Table: User_City_Province                                    */
/*==============================================================*/
Create View User_City_Province(userName, city, province)
As Select
        userinfo.userName,
        city.cityName,
        province.provinceName
   From
        userinfo,city,province
   Where
        userinfo.cityID = city.cityID
    AND city.provinceID = province.provinceID;

alter table AdoptApplication add constraint FK_adopt_applicatiom foreign key (adoptID)
      references AdoptInfo (adoptID) on delete restrict on update restrict;

alter table AdoptApplication add constraint FK_pet_application foreign key (petID)
      references Pet (petID) on delete restrict on update restrict;

alter table AdoptApplication add constraint FK_user_application foreign key (userAccount)
      references UserInfo (userAccount) on delete restrict on update restrict;

alter table City add constraint FK_city_province foreign key (provinceID)
      references province (provinceID) on delete restrict on update restrict;

alter table Pet add constraint FK_pet_adopt foreign key (adoptID)
      references AdoptInfo (adoptID) on delete restrict on update restrict;

alter table Pet add constraint FK_pet_breed foreign key (breedID)
      references Breed (breedID) on delete restrict on update restrict;

alter table RevisitRecord add constraint FK_adopt_revisit foreign key (adoptID)
      references AdoptInfo (adoptID) on delete restrict on update restrict;

alter table RevisitRecord add constraint FK_pet_revisit foreign key (petID)
      references Pet (petID) on delete restrict on update restrict;

alter table UserInfo add constraint FK_user_city foreign key (cityID)
      references City (cityID) on delete restrict on update restrict;
      
/*==============================================================*/
/* Trigger: adopt_duration                                      */
/*==============================================================*/
Create Trigger adopt_duration Before Insert On AdoptInfo
	For Each Row Update AdoptInfo Set adoptTime = Now() - adoptDate;

DELIMITER //

/*==============================================================*/
/* Trigger: adopt_status                                        */
/*==============================================================*/
CREATE TRIGGER adopt_status AFTER INSERT ON adoptinfo
FOR EACH ROW BEGIN
    UPDATE pet
        SET pet.isAdopted = true
        WHERE new.petID = pet.petID;
    UPDATE adoptapplication
        SET adoptapplication.adoptID = new.adoptID
        WHERE new.applicationID = adoptapplication.applicationID;
	UPDATE adoptapplication
        SET adoptapplication.isApproved = true
        WHERE new.applicationID = adoptapplication.applicationID;
END//

/*==============================================================*/
/* Stored Procedures: AdopterHasPets                            */
/*==============================================================*/
create
    definer = root@localhost procedure AdopterHasPets(IN AdopterAccount int, OUT HasPets int)
PROC: BEGIN
    SELECT COUNT(*) INTO HasPets
    FROM  adoptapplication, pet
    WHERE AdopterAccount = adoptapplication.userAccount
    AND   adoptapplication.petID = pet.petID
    AND   adoptapplication.isApproved = true
    GROUP BY adoptapplication.adopterName;
    IF HasPets = null THEN SET HasPets = 0;
	End if;
end//