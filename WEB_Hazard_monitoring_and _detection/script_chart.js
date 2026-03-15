// Khởi tạo biểu đồ Chart.js
const ctx = document.getElementById('realtimeChart').getContext('2d');

const data = {
  labels: [],
  datasets: [
    {
      label: 'Nhiệt độ (°C)',
      borderColor: '#ff6384',
      backgroundColor: 'transparent',
      data: [],
      tension: 0.4,
      cubicInterpolationMode: 'monotone'
    },
    {
      label: 'Độ ẩm (%)',
      borderColor: '#36a2eb',
      backgroundColor: 'transparent',
      data: [],
      tension: 0.4,
      cubicInterpolationMode: 'monotone'
    },
    {
      label: 'Khí độc (ppm)',
      borderColor: '#ffcd56',
      backgroundColor: 'transparent',
      data: [],
      tension: 0.4,
      cubicInterpolationMode: 'monotone'
    },
    {
      label: 'Rung chấn (PGA)',
      borderColor: '#4bc0c0',
      backgroundColor: 'transparent',
      data: [],
      tension: 0.4,
      cubicInterpolationMode: 'monotone'
    }
  ]
};

const config = {
  type: 'line',
  data: data,
  options: {
    responsive: true,
    maintainAspectRatio: false,
    animation: {
      duration: 300,
      easing: 'easeOutQuart'
    },
    scales: {
      x: {
        title: { display: true, text: 'Thời gian', color: 'white' },
        ticks: { color: 'white' }
      },
      y: {
        beginAtZero: true,
        title: { display: true, text: 'Giá trị cảm biến', color: 'white' },
        ticks: { color: 'white' }
      }
    },
    plugins: {
      legend: { labels: { color: 'white' } }
    }
  }
};

const chart = new Chart(ctx, config);

// Hàm cập nhật biểu đồ mỗi 3 giây
function updateChartFromFirebase() {
  const now = new Date();
  const timeLabel = now.toLocaleTimeString('vi-VN');

  const ref = db.ref("Hazard_monitoring_and_detection/value");

  ref.once("value").then(snapshot => {
    const values = snapshot.val();
    if (!values) return;

    const node1 = values.Node_1 || {};
    const node2 = values.Node_2 || {};

    // Xoá dữ liệu cũ nếu vượt quá 20 điểm
    if (data.labels.length >= 20) {
      data.labels.shift();
      data.datasets.forEach(dataset => dataset.data.shift());
    }

    data.labels.push(timeLabel);
    data.datasets[0].data.push(Number(node2.value_temperature || 0));
    data.datasets[1].data.push(Number(node2.value_humidity || 0));
    data.datasets[2].data.push(Number(node1.value_ppm || 0));
    data.datasets[3].data.push(Number(node1.value_pga || 0));

    chart.update();
  }).catch(err => {
    console.error("Lỗi lấy dữ liệu từ Firebase:", err);
  });
}

let isPaused = false;
const toggleButton = document.getElementById('toggleButton');

toggleButton.addEventListener('click', () => {
  isPaused = !isPaused;
  toggleButton.textContent = isPaused ? '▶️ Run' : '⏸ Pause';
});

// Hàm cập nhật biểu đồ mỗi 3 giây
function updateChartFromFirebase() {
  if (isPaused) return; // Nếu đang pause thì không làm gì

  const now = new Date();
  const timeLabel = now.toLocaleTimeString('vi-VN');

  const ref = db.ref("Hazard_monitoring_and_detection/value");

  ref.once("value").then(snapshot => {
    const values = snapshot.val();
    if (!values) return;

    const node1 = values.Node_1 || {};
    const node2 = values.Node_2 || {};

    if (data.labels.length >= 20) {
      data.labels.shift();
      data.datasets.forEach(dataset => dataset.data.shift());
    }

    data.labels.push(timeLabel);
    data.datasets[0].data.push(Number(node2.value_temperature || 0));
    data.datasets[1].data.push(Number(node2.value_humidity || 0));
    data.datasets[2].data.push(Number(node1.value_ppm || 0));
    data.datasets[3].data.push(Number(node1.value_pga || 0));

    chart.update();
  }).catch(err => {
    console.error("Lỗi lấy dữ liệu từ Firebase:", err);
  });
}

setInterval(updateChartFromFirebase, 1000);

