const FIREBASE_URL = 'https://hazard-monitoring-detection-default-rtdb.firebaseio.com/Hazard_monitoring_and_detection';


// Hàm hiện ẩn dữ liệu
function toggleNode(nodeId) {
  const container = document.getElementById(nodeId);
  if (!container) return;

  const tbody = container.querySelector("tbody");
  if (!tbody) return;

  // Tìm nút trong hàng cuối cùng của thead (nơi gọi toggleNode)
  const button = container.querySelector("thead button");

  // Toggle hiển thị dữ liệu
  if (tbody.style.display === "none" || tbody.style.display === "") {
    tbody.style.display = "table-row-group"; // Hiện dữ liệu
    if (button) button.textContent = "Ẩn Dữ Liệu";
  } else {
    tbody.style.display = "none"; // Ẩn dữ liệu
    if (button) button.textContent = "Hiện Dữ Liệu";
  }
}


// Hàm lấy thời gian định dạng dd-mm-yyyy hh:mm:ss
function getFormattedTimestamp() {
  const now = new Date();
  const yyyy = now.getFullYear();
  const mm = String(now.getMonth() + 1).padStart(2, '0');
  const dd = String(now.getDate()).padStart(2, '0');
  const hh = String(now.getHours()).padStart(2, '0');
  const min = String(now.getMinutes()).padStart(2, '0');
  const ss = String(now.getSeconds()).padStart(2, '0');
  return `${dd}-${mm}-${yyyy} ${hh}:${min}:${ss}`;
}

// Hàm cập nhật lịch sử cho Node_1 và Node_2 với timestamp đồng nhất
async function updateHistory() {
  try {
    // Lấy dữ liệu hiện tại từ Firebase
    const valueRes = await fetch(`${FIREBASE_URL}/value.json`);
    const deviceRes = await fetch(`${FIREBASE_URL}/device.json`);
    const valueData = await valueRes.json();
    const deviceData = await deviceRes.json();

    // Lấy timestamp duy nhất cho cả 2 node
    const timestamp = getFormattedTimestamp();

    // Chuẩn bị dữ liệu cho Node_1
    const node1 = {
      timestamp: timestamp,
      temp: valueData.Node_2?.value_temperature ?? null,
      humid: valueData.Node_2?.value_humidity ?? null,
      buzzer1: deviceData.Node_1?.buzzer_1 === 1 ? 'ON' : 'OFF'
    };

    // Chuẩn bị dữ liệu cho Node_2
    const node2 = {
      timestamp: timestamp,
      gas: valueData.Node_1?.value_ppm ?? null,
      pga: valueData.Node_1?.value_pga ?? null,
      buzzer2: deviceData.Node_2?.buzzer_2 === 1 ? 'ON' : 'OFF'
    };

    // Gửi PUT song song 2 node với timestamp giống nhau
    await Promise.all([
      fetch(`${FIREBASE_URL}/history/Node_1/${timestamp}.json`, {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(node1)
      }),
      fetch(`${FIREBASE_URL}/history/Node_2/${timestamp}.json`, {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(node2)
      })
    ]);

    console.log('Cập nhật lịch sử thành công lúc', timestamp);
  } catch (error) {
    console.error('Lỗi cập nhật lịch sử:', error);
  }
}


// Các biến phân trang mặc định
let currentPageNode1 = 1;
let rowsPerPageNode1 = 5;
let currentPageNode2 = 1;
let rowsPerPageNode2 = 5;

// Biến lưu cache dữ liệu để không fetch lại nhiều lần
let cacheNode1 = [];
let cacheNode2 = [];

// Hàm lấy dữ liệu lịch sử từ Firebase và trả về object đã được sắp xếp theo thời gian giảm dần
async function fetchHistory(node) {
  try {
    const res = await fetch(`${FIREBASE_URL}/history/${node}.json`);
    const data = await res.json();
    if (!data) return [];

    // Chuyển object thành array và sort theo timestamp giảm dần
    const arr = Object.entries(data).map(([key, value]) => {
      return {
        timestamp: key,
        ...value
      };
    });

    // Sửa lại hàm sort đúng: chuyển timestamp định dạng "dd-MM-yyyy HH:mm:ss" thành Date để so sánh
    arr.sort((a, b) => {
      const dateA = parseDateString(a.timestamp);
      const dateB = parseDateString(b.timestamp);
      return dateB - dateA; // giảm dần
    });

    return arr;
  } catch (error) {
    console.error('Lỗi fetch lịch sử:', error);
    return [];
  }
}

// Hàm parse string định dạng dd-MM-yyyy HH:mm:ss thành Date
function parseDateString(dateStr) {
  // "dd-MM-yyyy HH:mm:ss"
  const [datePart, timePart] = dateStr.split(' ');
  const [dd, MM, yyyy] = datePart.split('-').map(Number);
  const [hh, mm, ss] = timePart.split(':').map(Number);
  return new Date(yyyy, MM - 1, dd, hh, mm, ss);
}

// Hàm xóa lịch sử Firebase cho node
async function deleteHistory(node) {
  try {
    const res = await fetch(`${FIREBASE_URL}/history/${node}.json`, {
      method: 'DELETE'
    });
    if (!res.ok) throw new Error('Xóa lịch sử thất bại');
    return true;
  } catch (error) {
    console.error(error);
    alert('Xóa lịch sử không thành công!');
    return false;
  }
}

// Hàm hiển thị dữ liệu phân trang lên bảng Node 1
function renderTableNode1(data, page, rowsPerPage) {
  const tbody = document.getElementById('historyNode1');
  tbody.innerHTML = '';

  const start = (page - 1) * rowsPerPage;
  const end = Math.min(start + rowsPerPage, data.length);

  for (let i = start; i < end; i++) {
    const row = data[i];
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td>${row.timestamp}</td>
      <td>${row.temp !== null ? row.temp : ''}</td>
      <td>${row.humid !== null ? row.humid : ''}</td>
      <td>${row.buzzer1}</td>
      <td></td>
    `;
    tbody.appendChild(tr);
  }
}

// Hàm hiển thị dữ liệu phân trang lên bảng Node 2
function renderTableNode2(data, page, rowsPerPage) {
  const tbody = document.getElementById('historyNode2');
  tbody.innerHTML = '';

  const start = (page - 1) * rowsPerPage;
  const end = Math.min(start + rowsPerPage, data.length);

  for (let i = start; i < end; i++) {
    const row = data[i];
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td>${row.timestamp}</td>
      <td>${row.gas !== null ? row.gas : ''}</td>
      <td>${row.pga !== null ? row.pga : ''}</td>
      <td>${row.buzzer2}</td>
      <td></td>
    `;
    tbody.appendChild(tr);
  }
}

// Hàm tạo controls phân trang và nút xóa lịch sử cho Node 1
function createPaginationControlsNode1(totalRows) {
  const containerId = 'paginationNode1';
  let container = document.getElementById(containerId);

  if (!container) {
    container = document.createElement('div');
    container.id = containerId;
    container.className = 'pagination-controls';
    document.getElementById('node1').appendChild(container);
  }

  container.innerHTML = `
    <button id="prevNode1">Previous</button>
    <span>Page ${currentPageNode1}</span>
    <button id="nextNode1">Next</button>
    <select id="rowsPerPageNode1">
      <option value="5" ${rowsPerPageNode1 === 5 ? 'selected' : ''}>5</option>
      <option value="10" ${rowsPerPageNode1 === 10 ? 'selected' : ''}>10</option>
      <option value="20" ${rowsPerPageNode1 === 20 ? 'selected' : ''}>20</option>
    </select> rows per page
    <button id="deleteHistoryNode1" style="margin-left:10px; color:red;">Xóa Lịch Sử</button>
  `;

  document.getElementById('prevNode1').onclick = () => {
    if (currentPageNode1 > 1) {
      currentPageNode1--;
      renderAllNode1();
    }
  };
  document.getElementById('nextNode1').onclick = () => {
    if (currentPageNode1 * rowsPerPageNode1 < totalRows) {
      currentPageNode1++;
      renderAllNode1();
    }
  };
  document.getElementById('rowsPerPageNode1').onchange = (e) => {
    rowsPerPageNode1 = parseInt(e.target.value);
    currentPageNode1 = 1;
    renderAllNode1();
  };

  document.getElementById('deleteHistoryNode1').onclick = async () => {
    if (confirm('Bạn có chắc muốn xóa toàn bộ lịch sử Node 1?')) {
      const success = await deleteHistory('Node_1');
      if (success) {
        cacheNode1 = [];
        currentPageNode1 = 1;
        // Chỉ render lại bảng, không gọi loadAndRender để tránh gọi updateHistory và tải lại nhiều lần
        await renderAllNode1();
        alert('Đã xóa lịch sử Node 1 thành công!');
      }
    }
  };
}

// Hàm tạo controls phân trang và nút xóa lịch sử cho Node 2
function createPaginationControlsNode2(totalRows) {
  const containerId = 'paginationNode2';
  let container = document.getElementById(containerId);

  if (!container) {
    container = document.createElement('div');
    container.id = containerId;
    container.className = 'pagination-controls';
    document.getElementById('node2').appendChild(container);
  }

  container.innerHTML = `
    <button id="prevNode2">Previous</button>
    <span>Page ${currentPageNode2}</span>
    <button id="nextNode2">Next</button>
    <select id="rowsPerPageNode2">
      <option value="5" ${rowsPerPageNode2 === 5 ? 'selected' : ''}>5</option>
      <option value="10" ${rowsPerPageNode2 === 10 ? 'selected' : ''}>10</option>
      <option value="20" ${rowsPerPageNode2 === 20 ? 'selected' : ''}>20</option>
    </select> rows per page
    <button id="deleteHistoryNode2" style="margin-left:10px; color:red;">Xóa Lịch Sử</button>
  `;

  document.getElementById('prevNode2').onclick = () => {
    if (currentPageNode2 > 1) {
      currentPageNode2--;
      renderAllNode2();
    }
  };
  document.getElementById('nextNode2').onclick = () => {
    if (currentPageNode2 * rowsPerPageNode2 < totalRows) {
      currentPageNode2++;
      renderAllNode2();
    }
  };
  document.getElementById('rowsPerPageNode2').onchange = (e) => {
    rowsPerPageNode2 = parseInt(e.target.value);
    currentPageNode2 = 1;
    renderAllNode2();
  };

  document.getElementById('deleteHistoryNode2').onclick = async () => {
    if (confirm('Bạn có chắc muốn xóa toàn bộ lịch sử Node 2?')) {
      const success = await deleteHistory('Node_2');
      if (success) {
        cacheNode2 = [];
        currentPageNode2 = 1;
        await renderAllNode2();
        alert('Đã xóa lịch sử Node 2 thành công!');
      }
    }
  };
}

// Hàm tổng hợp tải dữ liệu Node 1 và hiển thị phân trang
async function renderAllNode1() {
if (cacheNode1.length === 0) {
cacheNode1 = await fetchHistory('Node_1');
}
renderTableNode1(cacheNode1, currentPageNode1, rowsPerPageNode1);
createPaginationControlsNode1(cacheNode1.length);
}

// Hàm tổng hợp tải dữ liệu Node 2 và hiển thị phân trang
async function renderAllNode2() {
if (cacheNode2.length === 0) {
cacheNode2 = await fetchHistory('Node_2');
}
renderTableNode2(cacheNode2, currentPageNode2, rowsPerPageNode2);
createPaginationControlsNode2(cacheNode2.length);
}

// Hàm gọi cập nhật lịch sử mới, rồi cập nhật bảng (cache trống để tải lại từ Firebase)
async function loadAndRender() {
await updateHistory();
cacheNode1 = [];
cacheNode2 = [];
currentPageNode1 = 1;
currentPageNode2 = 1;
await renderAllNode1();
await renderAllNode2();
}

// Chỉ gọi update history mỗi 60 giây (60000ms)
setInterval(loadAndRender, 60000);
// Gọi lần đầu load dữ liệu lịch sử để hiển thị ngay khi mở trang
loadAndRender();
