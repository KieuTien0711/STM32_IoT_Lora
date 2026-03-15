// ======== Tham chiếu DOM =========
const gasNode1 = document.getElementById('gasNode1');
const pgaNode1 = document.getElementById('pgaNode1');
const tempNode2 = document.getElementById('tempNode2');
const humidNode2 = document.getElementById('humidNode2');
const buzzerStatusNode1 = document.getElementById('buzzer-status-node1');
const buzzerStatusNode2 = document.getElementById('buzzer-status-node2');
const btnOnNode1 = document.getElementById('btn-on-node1');
const btnOffNode1 = document.getElementById('btn-off-node1');
const btnOnNode2 = document.getElementById('btn-on-node2');
const btnOffNode2 = document.getElementById('btn-off-node2');
const gasThreshold = document.getElementById('gasThreshold');
const pgaThreshold = document.getElementById('pgaThreshold');
const humidThreshold = document.getElementById('humidThreshold');
const tempThreshold = document.getElementById('tempThreshold');

// ========= Trạng thái hệ thống =========
let overrideBuzzerNode1 = null;
let overrideBuzzerNode2 = null;
let gasAlertOn = false, pgaAlertOn = false, tempAlertOn = false, humidAlertOn = false;

// ========= Cập nhật trạng thái giao diện =========
function capNhatTrangThaiBuzzer(elem, isOn) {
  elem.textContent = isOn ? "ĐANG BẬT" : "ĐANG TẮT";
  elem.classList.toggle("on", isOn);
  elem.classList.toggle("off", !isOn);
}

// ========= Nhập ngưỡng =========
function handleThresholdInput(event, type) {
  if (event.key !== "Enter") return;
  event.preventDefault();

  let value = parseFloat(event.target.value);
  if (isNaN(value)) {
    alert("Giá trị không hợp lệ!");
    return;
  }

  console.log(`handleThresholdInput called for ${type} with value ${value}`);

  const map = {
    temp: ["Node_2", "threshold_temperature", "Nhiệt độ"],
    humid: ["Node_2", "threshold_humidity", "Độ ẩm"],
    gas: ["Node_1", "threshold_ppm", "Khí gas"],
    pga: ["Node_1", "threshold_pga", "PGA"]
  };

  const [node, key, label] = map[type];
  db.ref(`Hazard_monitoring_and_detection/threshold/${node}/${key}`)
    .set(value)
    .then(() => alert(`Cập nhật ngưỡng ${label} thành công! Giá trị mới: ${value}`))
    .catch(err => {
      console.error(err);
      alert("Lỗi cập nhật ngưỡng!");
    });
}

// ========= Hàm cập nhật trạng thái từ realtime =========
let realtimeListenerRegistered = false;
function capNhatGiaoDienRealtime() {
  if (realtimeListenerRegistered) return; // tránh đăng ký listener nhiều lần
  realtimeListenerRegistered = true;

  const rootRef = db.ref("Hazard_monitoring_and_detection");

  rootRef.on("value", snapshot => {
    const data = snapshot.val();
    if (!data) return;

    const v1 = data.value?.Node_1;
    const v2 = data.value?.Node_2;
    const th1 = data.threshold?.Node_1;
    const th2 = data.threshold?.Node_2;
    const buzz1 = data.device?.Node_1?.buzzer_1;
    const buzz2 = data.device?.Node_2?.buzzer_2;

    let shouldBuzz1 = false;
    let shouldBuzz2 = false;

    // ========== Node 1 ==========
    if (v1 && th1) {
      const gas = parseFloat(v1.value_ppm), gasThres = parseFloat(th1.threshold_ppm);
      const pga = parseFloat(v1.value_pga), pgaThres = parseFloat(th1.threshold_pga);

      gasNode1.textContent = `${gas} ppm`;
      pgaNode1.textContent = `${pga} PGA`;
      gasThreshold.value = gasThres;
      pgaThreshold.value = pgaThres;

      if (gas > gasThres) {
        shouldBuzz1 = true;
        if (!gasAlertOn) {
          gasAlertOn = true;
          alert(`Cảnh báo Node 1: Khí gas vượt ngưỡng!\nGiá trị: ${gas} ppm`);
        }
      } else gasAlertOn = false;

      if (pga > pgaThres) {
        shouldBuzz1 = true;
        if (!pgaAlertOn) {
          pgaAlertOn = true;
          alert(`Cảnh báo Node 1: PGA vượt ngưỡng!\nGiá trị: ${pga}`);
        }
      } else pgaAlertOn = false;
    }

    // ========== Node 2 ==========
    if (v2 && th2) {
      const temp = parseFloat(v2.value_temperature), tempThres = parseFloat(th2.threshold_temperature);
      const humid = parseFloat(v2.value_humidity), humidThres = parseFloat(th2.threshold_humidity);

      tempNode2.textContent = `${temp} °C`;
      humidNode2.textContent = `${humid} %`;
      tempThreshold.value = tempThres;
      humidThreshold.value = humidThres;

      if (temp > tempThres) {
        shouldBuzz2 = true;
        if (!tempAlertOn) {
          tempAlertOn = true;
          alert(`Cảnh báo Node 2: Nhiệt độ vượt ngưỡng!\nGiá trị: ${temp} °C`);
        }
      } else tempAlertOn = false;

      if (humid > humidThres) {
        shouldBuzz2 = true;
        if (!humidAlertOn) {
          humidAlertOn = true;
          alert(`Cảnh báo Node 2: Độ ẩm vượt ngưỡng!\nGiá trị: ${humid} %`);
        }
      } else humidAlertOn = false;
    }

    // ==== Tự bỏ override nếu vượt ngưỡng ====
    if (shouldBuzz1 && overrideBuzzerNode1 !== null) overrideBuzzerNode1 = null;
    if (shouldBuzz2 && overrideBuzzerNode2 !== null) overrideBuzzerNode2 = null;

    const finalBuzz1 = overrideBuzzerNode1 !== null ? overrideBuzzerNode1 === 1 : shouldBuzz1;
    const finalBuzz2 = overrideBuzzerNode2 !== null ? overrideBuzzerNode2 === 1 : shouldBuzz2;

    // ==== Cập nhật trạng thái còi thực tế ====
    if (buzz1 !== undefined) {
      if (buzz1 !== (finalBuzz1 ? 1 : 0)) setBuzzerFromRealtime("Node_1", finalBuzz1);
      else capNhatTrangThaiBuzzer(buzzerStatusNode1, buzz1 === 1);
    }

    if (buzz2 !== undefined) {
      if (buzz2 !== (finalBuzz2 ? 1 : 0)) setBuzzerFromRealtime("Node_2", finalBuzz2);
      else capNhatTrangThaiBuzzer(buzzerStatusNode2, buzz2 === 1);
    }
  }, err => console.error("Realtime error:", err));
}

// ========= Nút điều khiển override =========
function setBuzzer(node, status) {
  if (node === "Node_1") {
    if (gasAlertOn || pgaAlertOn) {
      alert("Đang vượt ngưỡng Node 1, không thể thay đổi trạng thái còi bằng nút!");
      return;
    }
    overrideBuzzerNode1 = status;
  } else {
    if (tempAlertOn || humidAlertOn) {
      alert("Đang vượt ngưỡng Node 2, không thể thay đổi trạng thái còi bằng nút!");
      return;
    }
    overrideBuzzerNode2 = status;
  }

  db.ref(`Hazard_monitoring_and_detection/device/${node}/buzzer_${node === "Node_1" ? 1 : 2}`)
    .set(status)
    .catch(err => console.error("Lỗi cập nhật còi:", err));

  if (node === "Node_1") capNhatTrangThaiBuzzer(buzzerStatusNode1, status === 1);
  else capNhatTrangThaiBuzzer(buzzerStatusNode2, status === 1);
}

// ========= Cập nhật theo realtime (không override) =========
function setBuzzerFromRealtime(node, status) {
  db.ref(`Hazard_monitoring_and_detection/device/${node}/buzzer_${node === "Node_1" ? 1 : 2}`)
    .set(status ? 1 : 0)
    .catch(err => console.error("Realtime set buzzer error:", err));

  if (node === "Node_1") capNhatTrangThaiBuzzer(buzzerStatusNode1, status);
  else capNhatTrangThaiBuzzer(buzzerStatusNode2, status);
}

// ========= Đăng ký sự kiện =========
btnOnNode1.addEventListener("click", () => setBuzzer("Node_1", 1));
btnOffNode1.addEventListener("click", () => setBuzzer("Node_1", 0));
btnOnNode2.addEventListener("click", () => setBuzzer("Node_2", 1));
btnOffNode2.addEventListener("click", () => setBuzzer("Node_2", 0));

gasThreshold.addEventListener("keypress", e => handleThresholdInput(e, "gas"));
pgaThreshold.addEventListener("keypress", e => handleThresholdInput(e, "pga"));
tempThreshold.addEventListener("keypress", e => handleThresholdInput(e, "temp"));
humidThreshold.addEventListener("keypress", e => handleThresholdInput(e, "humid"));

// ========= Khởi tạo =========
capNhatGiaoDienRealtime();
