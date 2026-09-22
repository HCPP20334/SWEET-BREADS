const fs = require('fs');
const express = require('express');
const path = require('path');

// not used/ GameEngine сам управляет данными
function de_obfs(s){
     console.log("str=",s);
	obxrac32b64.decode(s.toString('utf16'),k);
}

const app = express();
const jsonpath = path.join('test.json');
var json_data = [];
app.use(express.static(__dirname));
app.get("/table",(req,res)=>{
    console.log(" call /api/table");
 //let jsondata = fs.readFileSync(jsonpath,"utf-8");
 res.send(json_data);
});
app.get("/table_write", (req, res) => {
    var usero = req.query.user;
    var scoreo = req.query.score;
    var timestampo = new Date();
    var diffo = req.query.diff;

    if (!usero || scoreo === undefined) {
        return res.status(400).send("Invalid parameters");
    }

    // Ищем, есть ли уже этот пользователь в таблице
    let existingIndex = json_data.findIndex(item => item.user === usero);

    if (existingIndex !== -1) {
        // Пользователь уже есть — обновляем его результат (перезаписываем)
        json_data[existingIndex].score = scoreo;
        json_data[existingIndex].diff = diffo;
        json_data[existingIndex].timestamp = timestampo;
        console.log(`Updated user ${usero}: score=${scoreo}`);
    } else {
        // Проверяем лимит на количество одинаковых score (если нужно сохранить это правило)
        let sameScoreCount = json_data.filter(item => item.score === scoreo).length;

        if (sameScoreCount >= 3) {
            console.log(`Score '${scoreo}' rejected: already exists 3 or more times.`);
            return res.status(400).send("Score limit reached for this value");
        }

        console.log(`New user ${usero}, score=${scoreo}`);

        // Добавляем нового пользователя
        json_data.push({
            user: usero,
            score: scoreo,
            diff: diffo,
            timestamp: timestampo
        });
    }

    // Сортировка от лучших к худшим
    json_data.sort((a, b) => b.score.localeCompare(a.score));

    // Ограничение до 20 результатов (оставляем только топ-20)
    if (json_data.length > 20) {
        json_data = json_data.slice(0, 20);
    }

    res.status(200).send("sending!");
});app.listen(4435, '0.0.0.0', () => {
    console.log("server started to 4435");
});
