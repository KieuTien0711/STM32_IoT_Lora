// firebase-config.js

// Cấu hình Firebase (đảm bảo các giá trị đúng theo dự án của bạn)
const firebaseConfig = {
    apiKey: "AIzaSyBFRVtsaj-mKcRrEgg5s8z7PcIGospzNn4",
    authDomain: "hazard-monitoring-detection.firebaseapp.com",
    // URL database phải đúng; có thể bạn cần cập nhật nếu sử dụng region khác
    databaseURL: "https://hazard-monitoring-detection-default-rtdb.firebaseio.com",
    projectId: "hazard-monitoring-detection",
    // Sửa storageBucket nếu cần: tránh lỗi ".firebasestorage.app" (thông thường là ".appspot.com")
    storageBucket: "hazard-monitoring-detection.appspot.com",
    messagingSenderId: "366551566111",
    appId: "1:366551566111:web:64e759b042c7c0da2c522a"
  };
  
  // Khởi tạo Firebase
  firebase.initializeApp(firebaseConfig);
  
  // Tạo đối tượng database và gắn vào window để có thể truy cập từ file khác
  const db = firebase.database();
  window.db = db;
  