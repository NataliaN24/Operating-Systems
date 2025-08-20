Вашите колеги от съседната лаборатория имат няколко GNU/Linux машини със специфични RAID
контролери и ползват специализирана система за наблюдение Hugin. Помогнете на колегите си, като
напишете shell скрипт, който взима температурата на дисковете, закачени към тези контролери, и
генерира изход, удобен за ползване от системата за наблюдение.
За достъп до данните на дисковете, колегите ви казват, че root потребителя може да изпълни следната
команда, която взима информация от даден контролер:
ssacli ctrl slot=x pd all show detail
където 𝑥 е номер на PCI слот, в който е включен контролера. Примерен изход от командата:
Smart Array P420i in Slot 0 (Embedded)
Array A
physicaldrive 1I:2:1
Rotational Speed: 15000
Current Temperature (C): 35
physicaldrive 1I:2:2
Interface Type: SAS
Current Temperature (C): 36
SMR Support: None
physicaldrive 1I:2:3
Status: OK
Drive Type: Spare Drive
Current Temperature (C): 34
Maximum Temperature (C): 42
28
Още един пример:
Smart Array P822 in Slot 1
Array J
physicaldrive 2E:1:19
Current Temperature (C): 29
physicaldrive 2E:1:20
Current Temperature (C): 28
Unassigned
physicaldrive 2E:1:21
Current Temperature (C): 30
physicaldrive 2E:1:22
Current Temperature (C): 29
Ако обобщим, изходът изглежда така:
• обща информация за контролера в съответния слот, и частност, модел на контролера (P420i/P822)
• една или повече секции на второ ниво, дефиниращи (нула или повече) масиви или (нула или
една) секция с неизползвани дискове (Array A/Array J/Unassigned), като буквата (A, J, ...) дефинира името на масива в рамките на контролера
• във всяка такава секция има една или повече подсекции за дискове в съответния масив (physicaldrive
1I:2:1, physicaldrive 2E:1:22, и т.н.), където низът след physicaldrive е име на този диск в
рамките на контролера
• във всяка подсекция за даден диск има множество параметри във вида key name: value като за
колегите ви е интересен само параметърът Current Temperature (C)
Нека въведем следните термини:
• идентификатор на диск – низ във вида “SSAcmmmappp”, където:
– SSA – литерал
– 𝑐 – номер на слот
– 𝑚𝑚𝑚 – модел на контролер
– 𝑎 – име на масив или UN при неизползван диск
– 𝑝𝑝𝑝 – име на диск, без двуеточия
• label на диск – низ във вида “SSAc mmm а qqq”, където
– 𝑞𝑞𝑞 – име на диск
– останалите полета са по горната дефиниция
• забележка: кавичките не са част от идентификатора или от label-а на даден диск.
Възможно е да съществува environment променлива CTRLSLOTS, чиято стойност да е един или повече
номера на PCI слотове, разделени с интервали, и в този случай скриптът трябва да извежда информация от тези контролери. Ако такава променлива не е дефинирана, скриптът по подразбиране да
работи с контролера в слот 0.
Скриптът трябва да поддържа следните видове изпълнение:
• с един параметър autoconf – скриптът трябва да извежда низа yes
• с един параметър config – скриптът трябва да извежда следния изход:
graph_title SSA drive temperatures
graph_vlabel Celsius
graph_category sensors
graph_info This graph shows SSA drive temp
SSA0P420iA1I21.label SSA0 P420i A 1I:2:1
SSA0P420iA1I21.type GAUGE
SSA0P420iA1I22.label SSA0 P420i A 1I:2:2
SSA0P420iA1I22.type GAUGE
SSA0P420iA1I23.label SSA0 P420i A 1I:2:3
SSA0P420iA1I23.type GAUGE
29
SSA1P822J2E119.label SSA1 P822 J 2E:1:19
SSA1P822J2E119.type GAUGE
SSA1P822J2E120.label SSA1 P822 J 2E:1:20
SSA1P822J2E120.type GAUGE
SSA1P822UN2E121.label SSA1 P822 UN 2E:1:21
SSA1P822UN2E121.type GAUGE
SSA1P822UN2E122.label SSA1 P822 UN 2E:1:22
SSA1P822UN2E122.type GAUGE
• без параметри – скриптът трябва да извежда следния изход:
SSA0P420iA1I21.value 35
SSA0P420iA1I22.value 36
SSA0P420iA1I23.value 34
SSA1P822J2E119.value 29
SSA1P822J2E120.value 28
SSA1P822UN2E121.value 30
SSA1P822UN2E122.value 29
В горните изходи:
• низовете, започващи с graph са литерали
• низовете, в които се среща .label, .type или .value започват с идентификатор на диск
• низът след .label е label на диск
• .type винаги е GAUGE
• числото след .value е температурата на въпросния диск

#!/bin/bash

[[ $# -ne 1 || ! -d $1 ]] && echo "1 param expected that is a dir" && exit 1

DIR=$(find $1/cfg -maxdepth 0 -type d)
[[ -z $DIR ]] && echo "cfg directory not foun" && exit 2

validate=$(find "$1/validate.sh" -executable)
[[ -z $validate ]] && echo "validate script not found" && exit 3

passFile=$(find "$1/foo.pwd" -type f)
[[ -z $passFile ]] && echo "password file not found" && exit 4

configFile=$(find "$1/foo.conf" -type f)
[[ -z $configFile ]] && echo "password file not found" && exit 5

files=$(find $DIR -type f -name '*.cfg')

echo "" > $configFile
while read file; do
	basename=$(basename $file)

	output=$($validate $file)
	exitCode=$?

	[[ $exitCode -eq 2 ]] && echo "Problem validating file: $file" && continue
	if [[ $exitCode -eq 1 ]]; then
		echo "$output" | awk -v name="$basename" '{ print name":"$0 }' 1>&2
		continue
	fi
	
	cat $file >> $configFile

	username=$(echo "$basename" | sed -E 's/\.cfg$//g')

	userLine=$(egrep "^$username" $passFile)
	[[ ! -z $userLine ]] && continue

	newPass=$(pwgen 10 1)
	hashedPass=$(mkpasswd $newPass)

	echo "${username}:${newPass}"
	echo "${username}:${hashedPass}" >> $passFile
done < <(echo "$files")
